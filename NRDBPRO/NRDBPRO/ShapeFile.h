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

#if !defined(AFX_SHAPEFILE_H__CBDF2FC0_F579_11D4_A746_0080AD88D050__INCLUDED_)
#define AFX_SHAPEFILE_H__CBDF2FC0_F579_11D4_A746_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

class CMapLayer;
class CQuery;
class CMapLayerObj;
class CArrayAttrSel;
struct CRectDbl;

//////////////////////////////////////////////////////////////////////

enum 
{
   SHPNullShape=0,
   SHPPoint=1,
   SHPPolyLine=3,
   SHPPolygon=5,
   SHPMultipoint=8,
   SHPPointZ=11,
   SHPPolyLineZ=13,
   SHPPolygonZ=15,
   SHPMultiPointZ=18,
   SHPPointM=21,
   SHPPolyLineM=23,
   SHPPolygonM=25,
   SHPMultipointM=28,
   SHPMultiPatch=31
};

#pragma pack(1)

struct CMainHeader
{
   int m_nFileCode;  // Big Endian
   int m_nUnused1;   // ""
   int m_nUnused2;
   int m_nUnused3;
   int m_nUnused4;
   int m_nUnused5;
   int m_nFileLength;
   int m_nVersion;   // Little endian
   int m_nShapeType; // ""
   double m_dXMin;
   double m_dYMin;
   double m_dXMax;
   double m_dYMax;
   double m_dZMin;
   double m_dZMax;
   double m_dMMin;
   double m_dMMax;   
};

struct CRecordHeader
{
   int m_nRecordNumber; // Big Endian
   int m_nContentLength; // ""
};

struct CIndexRecord
{
   int m_nOffset;
   int m_nContentLength;
};

struct CPolyLine
{
   CPolyLine();

   double m_dBox[4]; // Little endian
   DWORD m_nParts;     // ""
   DWORD m_nPoints;  
};

struct CShapePoint
{
   double m_dX; // Little endian
   double m_dY; 
};

struct CDBFHeader
{
   BYTE m_nValid;
   char m_aDate[3];
   int  m_nNumRecords;
   short  m_nHeaderBytes;
   short m_nRecordBytes;
   BYTE m_nReserved1[3];
   BYTE m_nReserved2[13];
   BYTE m_nReserved3[4];
};

struct CDBFDescriptor
{
   char m_sName[11];
   BYTE m_nType;
   BYTE m_nAddress[4];
   BYTE m_nFieldLength;
   BYTE m_nFieldCount;
   BYTE m_nReserved1[2];
   BYTE m_nWorkArea;
   BYTE m_nReserved2[2];
   BYTE m_nSetFieldsFlag;
   BYTE m_nReserved3[8];
};


struct CDBFRecordPoint
{
   char m_sID[8];      
   char m_sName[32];      
};

#define DBF_TEXT 32
#define DBF_NUMBER 16
#define DBF_NDP 7
#define DBF_BOOL 1
#define DBF_DATE 8

//////////////////////////////////////////////////////////////////////

class CShapeFile  
{
public:
	CShapeFile();
	virtual ~CShapeFile();

   BOOL ImportShapeFile(CLongLines* pMapLines, LPCSTR sFileName);   
   BOOL ExportShapeFile(CLongLines* pMapLines, LPCSTR sFileName, int nFlags);
   int  ImportShapeFileType(LPCSTR sFileName);
   BOOL ExportShapeFile(CMapLayer* pMapLayer, LPCSTR sFileName, int nFlags);

   BOOL ImportShapeFile(CMapLayer* pMapLayer, LPCSTR sFileName);

   void Convert();

   FILE *m_pFileIn, *m_pFileOut;
   CString m_sFileIn, m_sFileOut;
   BOOL m_bLatLon;
   CMainHeader m_mainheader;
   DWORD m_nShape;
   CRecordHeader m_recordheader;

   enum {LatLon=1, Header=2, PolyLine=4, Points=8, Polygon=16};

protected:
     
   BOOL ImportMultMapLines(CLongLines* pMapLines, CLongCoord, CLongLines*& pMapLinesNew);   
   BOOL ImportShapeFile(CLongLines* pMapLines, CCoordArray* pCoords, CMainHeader& mainheader, LPCSTR sFileName, int iFlag);
   BOOL ImportPolyLine(CLongLines* pMapLines, FILE* pFile, BOOL bLatLon, CRectDbl* pRect = NULL);
   BOOL ImportPoints(CCoordArray* pPoints, FILE* pFile, BOOL bLatLon);
   BOOL ExportPoints(CMapLayer* pMapLayer, FILE* pFile, BOOL bLatLon);
   BOOL ExportPolyLine(CMapLayer* pMapLayer, FILE* pFile, BOOL bLatLon, DWORD nShape);
   BOOL ExportShapeFilePoint(CMapLayer* pMapLayer, LPCSTR sFileName, BOOL bLatLon);
   BOOL ExportShapeFilePoly(CMapLayer* pMapLayer, LPCSTR sFileName, BOOL bLatLon);
   BOOL ExportIndexFile(CPolyLine& polyline, CMapLayer* pMapLayer, LPCSTR sFileName, CMainHeader& mainheader);
   BOOL ExportIndexFile(CMapLayer* pMapLayer, LPCSTR sFileName, CMainHeader& mainheader);
   BOOL ExportDBaseQuery(CMapLayer* pMapLayer, LPCSTR sFileName);
   BOOL ExportDBase(CMapLayer* pMapLayer, LPCSTR sFileName);
   BOOL IsValidMapObject(CAttrArray& aAttr, CArrayAttrSel& aAttrSel);
   int ReverseBytes(int);
   CPolyLine GetBoundingBoxPoly(CLongLines* pMapLines, BOOL bLatLon, int i1 = 0, int j2 = 0);
   CPolyLine GetBoundingBox(CMapLayer* pMapLayer, BOOL bLatLon);
   CPolyLine GetNumParts(CLongLines* pMapLines);
   CString GetLineText(CMapLayer*, CMapLayerObj*);
   void fwritex(void* p, size_t size, DWORD n, FILE* pFile);

   BOOL ConvertShapeFile();
   BOOL ConvertPolyLine();
   BOOL ConvertPoints();
   BOOL ConvertIndex();
   BOOL ConvertDBaseFile();

   BOOL PolylineToPolygon(CMapLayer*);   
};

#endif // !defined(AFX_SHAPEFILE_H__CBDF2FC0_F579_11D4_A746_0080AD88D050__INCLUDED_)
