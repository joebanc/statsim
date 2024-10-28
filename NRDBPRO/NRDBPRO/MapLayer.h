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

#ifndef _MAPLAYER_H_
#define _MAPLAYER_H_

#include "nrdb.h"
#include "query.h"
#include "imagefile.h"
#include "longarray.h"
#include "xmlfile.h"

#define DEFAULT_COLOUR RGB(10,192,10)

///////////////////////////////////////////////////////////////////////////////

class CMapLayerArray;


// Fixes compiler error

#pragma pack(1)

///////////////////////////////////////////////////////////////////////////////

struct CRectDbl
{
   double left, right, top, bottom;
   CRectDbl() {left=0;right=0;top=0;bottom=0;}
   double Width() {return right-left;}
   double Height() {return fabs(bottom-top);}
   double Area() {return fabs(Width() * Height());}
   BOOL IsEmpty() {return left == 0 && right == 0 && top == 0 && bottom == 0;}
};

struct CRectEx : public CRect
{
   CRectEx() {left=0;right=0;top=0;bottom=0;}

   double Area() {return fabs((double) (Width() * Height()));}
   BOOL IsEmpty() {return left == 0 && right == 0 && top == 0 && bottom == 0;}
   void Merge(CRectEx e) 
   {
	   left = min(left, e.left);
	   right = max(right, e.right);
	   bottom = min(bottom, e.bottom);
	   top = max(top, e.top);
   }
};

/////////////////////////////////////////////////////////////////////////////
//
// class CMapLayerObj
// 
// Contains either a point or a polyline (map lines) and corresponding 
// feature
//

class CMapLayerObj 
{
public:
   CMapLayerObj();   
   CMapLayerObj(CMapLayerObj&);   
   ~CMapLayerObj();      

   void SetMapObject(void* pMapObject)
   {ASSERT(m_pMapObject == NULL || pMapObject == NULL);m_pMapObject = pMapObject;}
   void* GetMapObject() {return m_pMapObject;}

   void SetDataType(int nDataType) {m_nDataType = nDataType;}
   int GetDataType() {return m_nDataType;}

   void SetFeature(long lFeature) {m_lFeature = lFeature;}
   long GetFeature() {return m_lFeature;}

   void FreeMem();

   void SetValue(double d) {m_dValue = d;}
   double GetValue() {return m_dValue;}

   LPCSTR GetText() {return m_sText;}
   void SetText(LPCSTR s) {m_sText = s;}

   void SetDate(long lDate) {m_lDate = lDate;}
   long GetDate() {return m_lDate;}

   CRect& GetExtent() {return m_rect;}      
   long& GetCentreX() {return m_dX;}
   long& GetCentreY() {return m_dY;}
      
protected:
   void* m_pMapObject;   
     
   CString m_sText;
   int m_nDataType;
   long m_lFeature;   
   double m_dValue;  
   CRectEx m_rect;
   long m_lDate;
   long m_dX, m_dY; //Centre
};

/////////////////////////////////////////////////////////////////////////////
//
// class CMapStyle
//
// Contains the aesthetic properties of a map layer
//

class CMapStyle
{
public:

   CMapStyle();
   CMapStyle(CMapStyle&);

   CMapStyle& operator=(const CMapStyle&);
   BOOL operator==(const CMapStyle&);

   CString AsString();
   BOOL StringAs(CString);

   void AsXML(CXMLObj*);
   BOOL XMLAs(CXMLObj*);

   int m_nLineStyle;
   int m_nLineWidth;   
   COLORREF m_crLine;
   COLORREF m_crFill;
   int m_nPattern;
   int m_nHatch;
   int m_nSymbol;
   double m_dSymSize;     
};

struct CStyleScheme
{
   CString m_sName;
   BOOL m_bSystem;
   CArray <CMapStyle, CMapStyle> m_aStyle;

   CStyleScheme() {m_bSystem = FALSE;};
   CStyleScheme(CStyleScheme &rSrc)
   {
      *this = rSrc;
   };
   CStyleScheme& operator=(CStyleScheme& rSrc)
   {
      m_bSystem = rSrc.m_bSystem;
      m_sName = rSrc.m_sName;
      m_aStyle.Copy(rSrc.m_aStyle);
      return *this;
   }

   BOOL XMLAs(CXMLObj*);
   
};

///////////////////////////////////////////////////////////////////////////////

struct CStyleSchemes : public CArray <CStyleScheme, CStyleScheme>
{
   BOOL XMLAs(CXMLObj*);
   void AsXML(CXMLObj&);
};

/////////////////////////////////////////////////////////////////////////////
//
// class CColourFeature
//
// Object associating a feature within a layer with a colour
//

class CColourFeature : public CMapStyle
{
public:
   long m_lFeatureId;
   CString m_sAttr;
   CString m_sFeature;   

   CColourFeature();
};

class CColourFeatureArray : public CArray <CColourFeature,CColourFeature>
{
public:
   void Write(FILE* pFile, int nSepColour);
   BOOL Read(FILE* pFile, int nSepColour); 

   void AsXML(CXMLObj*);
   BOOL XMLAs(CXMLObj*);
};

/////////////////////////////////////////////////////////////////////////////
//
// class CColourRange
//
// Contains map properties whereby a range of values e.g. 1 to 10 are 
// represented by one color
//

class CColourRange : public CMapStyle
{
public:
   double m_dMin;
   double m_dMax;       
};

class CColourRangeArray: public CArray <CColourRange, CColourRange>
{
public:
   void Write(FILE* pFile);
   BOOL Read(FILE* pFile); 

   CColourRangeArray() {m_dRound = 0; m_bApplyPercent = 0;}
   CColourRangeArray(CColourRangeArray&);
   CColourRangeArray& operator=(CColourRangeArray &rSrc);   

   CString GetDesc(int i);
   CString GetName() {return m_sName;}

   BOOL XMLAs(CXMLObj*);
   void AsXML(CXMLObj*);

   double m_dRound;
   BOOL m_bApplyPercent; // User edited schemes.xml, means to apply to whole range i.e. first 25% is blue etc.
   CString m_sName;
};

///////////////////////////////////////////////////////////////////////////////

struct CRangeSchemes : public CArray <CColourRangeArray, CColourRangeArray>
{
   BOOL XMLAs(CXMLObj*);
   void AsXML(CXMLObj&);
};

///////////////////////////////////////////////////////////////////////////////

class CMapLayoutObj
{
public:

   CMapLayoutObj();
   CMapLayoutObj(const CMapLayoutObj&);
   CMapLayoutObj& operator=(const class CMapLayoutObj&);

   void AsXML(CXMLObj*);
   BOOL XMLAs(CXMLObj*);

   CString GetTypeDesc();

   CRect m_rect;
   CMapStyle m_style;
   LOGFONT m_logfont;
   int m_nType;
   CString m_sImageFile;
   CString m_sText;
};

class CMapLayout
{
public:

   CMapLayout();
   CMapLayout(CMapLayout& rSrc);
   CMapLayout& operator=(CMapLayout& rSrc);

   static CRect RectAsPercent(CRect rectT, CRect rect);
   static CRect RectFromPercent(CRect rectT, CRect rectP);
   
   CMapLayoutObj GetLayoutObj(int nType);
   CMapLayoutObj* GetLayoutObjPtr(int nType);
   void SetLayoutObj(int nType, CMapLayoutObj);

   void AsXML(CXMLObj*);
   BOOL XMLAs(CXMLObj*);
   
   CString m_sName;
   BOOL m_bDefault;
   CArray <CMapLayoutObj, CMapLayoutObj> m_aLayout;

   enum {none, map, legend, title, arrow, source, scalebar, box, picture, text, projection, location, scale, locator};
};

///////////////////////////////////////////////////////////////////////////////

class CMapLayoutArray : public CArray <CMapLayout, CMapLayout>
{
public:

   CMapLayout GetDefault();
   BOOL IsAuto() {return GetSize() == 0 || GetAt(0).m_bDefault;}
   
   void AsXML(CXMLObj*);
   BOOL XMLAs(CXMLObj*);      

   static double m_dLayoutVersion;
};

///////////////////////////////////////////////////////////////////////////////

class CMapProperties : public CMapStyle
{
public:
   CString m_sName;        
   COLORREF m_colorText;      
   int m_bOverlap;
   int m_nTextPos; 
   CString m_sComment;
   BOOL m_bAutoColour;   
   BOOL m_bAutoSize;   
   BOOL m_bPolygon;
   BOOL m_bScaleFont;
   BOOL m_bBestFitSym;   
   BOOL m_bVisible;
   BOOL m_bShowLayerName;
   int m_nSepColour; 
   BOOL m_bRangeColour;
   double m_dAutoMin;
   double m_dAutoMax;
   LOGFONT m_logfont;  
   LOGFONT m_logfontC;   

   CColourFeatureArray m_aColourFeature;
   CColourRangeArray m_aRangeColour;   
   

   CMapProperties();
   CMapProperties(CMapProperties&);
   CMapProperties& operator=(CMapProperties&);
};

/////////////////////////////////////////////////////////////////////////////
//
// Defines how the grid is displayed

class CMapGrid
{
public:
   CMapGrid();
   CMapGrid(CMapGrid&);

   LOGFONT m_logfont;
   CMapStyle m_style;
   int m_nType;
   int m_nDegLat, m_nMinLat, m_nSecLat;
   int m_nDegLng, m_nMinLng, m_nSecLng;

   enum {none, defaultgrid, labelsgrid, labels};
};


/////////////////////////////////////////////////////////////////////////////
//
// Contains the map objects making up a layer
//

class CMapLayer : public CArray <CMapLayerObj*,  CMapLayerObj*>
{
   friend class CDlgAddOverlay;

public:

   CMapLayer(BOOL bMapLines = FALSE);   
   CMapLayer(CMapLayer&);   
   ~CMapLayer();

   void RemoveAll();
   
   BOOL Initialise(long lFType, CLongArray& alFeatures, CFTypeAttrArray& aAttr, COLORREF color);
   BOOL Initialise(CQuery* pQuery);

   LPCSTR GetName() {return m_prop.m_sName;}
   COLORREF GetColour() {if (m_prop.m_bPolygon) return m_prop.m_crFill; else return m_prop.m_crLine;}
   COLORREF GetTextColour() {return m_prop.m_colorText;}
   COLORREF GetColourLine() {return m_prop.m_crLine;}
   COLORREF GetColourFill() {return m_prop.m_crFill;}
   int GetSymbol() {return m_prop.m_nSymbol;}
   double GetSymSize() {return m_prop.m_dSymSize;}
   int GetPattern() {return m_prop.m_nPattern;}
   int GetHatch() {return m_prop.m_nHatch;}
   int GetLineStyle() {return m_prop.m_nLineStyle;}
   int GetLineWidth() {return m_prop.m_nLineWidth;}
   BOOL GetOverlap() {return m_prop.m_bOverlap;}
   BOOL GetScaleFont() {return m_prop.m_bScaleFont;}
   LOGFONT GetFont() {return m_prop.m_logfont;}
   LOGFONT GetFontC() {return m_prop.m_logfontC;}
   int GetTextPos() {return m_prop.m_nTextPos;}
   LPCSTR GetComment() {return m_prop.m_sComment;}
   BOOL GetBestFitSym() {return m_prop.m_bBestFitSym;}   
   BOOL IsVisible() {return m_prop.m_bVisible;}
   BOOL IsShowLayerName() {return m_prop.m_bShowLayerName;}
   void SetVisible(BOOL b) {m_prop.m_bVisible = b;}
   int GetSepColour() {return m_prop.m_nSepColour;}
   BOOL GetRangeColour() {return m_prop.m_bRangeColour;}
   BOOL GetAutoColour() {return m_prop.m_bAutoColour;}
   double GetAutoMin() {return m_prop.m_dAutoMin;}
   double GetAutoMax() {return m_prop.m_dAutoMax;}
   BOOL GetAutoSize() {return m_prop.m_bAutoSize;}
   BOOL IsPolygon() {return m_prop.m_bPolygon;}
   CMapProperties& GetMapProp() {return m_prop;}   

   BOOL IsExtentChecked() {return m_bExtentChecked;}
   void SetExtentChecked() {m_bExtentChecked = TRUE;}

   long GetFType() {return m_lFType;}   
   CQuery* GetQuery() {return m_pQuery;}
   void SetQuery(CQuery* p) {m_pQuery = p;}
   CArrayAttrSel& GetAttr() {return m_aAttr;}

   BOOL LoadData(BOOL bOnLoad = FALSE);   

   enum {BottomRight=0, TopLeft=1, TopRight=2, BottomLeft=3, Center=4,BestFit=5, 
   Left=-1,Right=-2,Top=-4,Bottom=-3, Start=-4, Null=-5};
   enum {SepColourFeature=1, SepColourAttr=2, LegendValues=3};

   void Write(FILE* pFile);
   BOOL Read(CMapLayerArray*, FILE* pFile);      

   void AsXML(CXMLObj*);
   BOOL XMLAs(CXMLObj*);

   void SetFileName(LPCSTR sFile) {m_sFile = sFile;}
   void SetColumnName(LPCSTR sColumn) {m_sColumn = sColumn;}
   LPCSTR GetFileName() {return m_sFile;} 

   BOOL m_bOnLoad;

protected:

   BOOL AddAttribute(CMapLayerObj* pMapLayerObj, CAttrArray& aAttr);   

// Member variables

   long m_lFType;
   CArrayAttrSel m_aAttr;
   CMapProperties m_prop;  
   BOOL m_bExtentChecked;
   CLongArray m_alFeatures;      
   CString m_sFile;
   CString m_sColumn;
   CQuery* m_pQuery;    
     
};
      
///////////////////////////////////////////////////////////////////////////////

class CMapLayerArray : public CArray <CMapLayer*, CMapLayer*>
{   
   friend class CDlgLayout;
   friend class CViewMap;
   friend class CViewLegend;
   friend class CPageMapLegendProp;
   friend class CMapLayer;

public:
   CMapLayerArray();
   ~CMapLayerArray();

   void Reset();

   void Write(LPCSTR sFile);
   int Read(LPCSTR sFile);

   void AsXML(CXMLObj*);
   BOOL XMLAs(CXMLObj*);

   void Update(CMapLayer*, CMapLayerObj* pMapLayerObj);
   void RemoveAll();
   void OnLoad(LPCSTR sFile = NULL);
   
   int GetDefault() {return m_iDefault;}
   void SetDefault(int i) {m_iDefault = i;}

   CMapLayerArray& operator=(CMapLayerArray&);
   
   LPCSTR GetTitle() {return m_sTitle;}
   LOGFONT GetFontT() {return m_logfontT;}
   LOGFONT GetFontL() {return m_logfontL;}
   BOOL GetLegend1Font() {return m_bLegend1Font;}

   CMapGrid GetMapGrid() {return m_mapgrid;}
   void SetMapGrid(CMapGrid mapgrid) {m_mapgrid = mapgrid;}

   CMapLayout GetMapLayout() {return m_layout;}
   void SetMapLayout(CMapLayout layout) {m_layout = layout;}

protected:
   static CString m_sError;
   int m_iDefault;

   CMapLayout m_layout;

   CMapGrid m_mapgrid;
  
   CString m_sTitle;
   LOGFONT m_logfontT; // title
   LOGFONT m_logfontL; //legend
   BOOL m_bLegend1Font;

};

///////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif
