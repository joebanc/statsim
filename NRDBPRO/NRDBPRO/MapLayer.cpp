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
#include <strstream>

#include "nrdb.h"
#include "maplayer.h"
#include "viewmap.h"
#include "importodbc.h"
#include "bdimportexport.h"
#include "dlgprogress.h"
#include "mainfrm.h"
#include "comboboxsymbol.h"
#include "dlgaddoverlay.h"
#include "docmap.h"
#include "projctns.h"
#include "spatial.h"

///////////////////////////////////////////////////////////////////////////////

CString CMapLayerArray::m_sError;
double CMapLayoutArray::m_dLayoutVersion;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMapLayerObj::CMapLayerObj()
{
   m_pMapObject = NULL;
   m_nDataType = 0;
   m_lFeature = 0;
   m_dValue = AFX_RFX_DOUBLE_PSEUDO_NULL;
   m_dX = 0; 
   m_dY = 0;
   //m_rect = CRect(0,0,0,0);
   m_lDate = 0;
}

CMapLayerObj::CMapLayerObj(CMapLayerObj& rSrc)
{
   m_nDataType = rSrc.m_nDataType;
   m_lFeature = rSrc.m_lFeature;   
   m_dValue = rSrc.m_dValue;
   m_lDate = rSrc.m_lDate;
   m_rect = rSrc.m_rect;
   m_dX = rSrc.m_dX;
   m_dY = rSrc.m_dY;

   m_pMapObject = NULL;   
}

CMapLayerObj::~CMapLayerObj()
{   
   FreeMem();
};

void CMapLayerObj::FreeMem()
{
   if (m_pMapObject == NULL) return;

   if (m_nDataType == BDMAPLINES)
   {
     //delete ((CLongLines*)m_pMapObject);
	   delete ((CSpatialPoly*)m_pMapObject);
   } 
   else if (m_nDataType == BDCOORD)
   {
     delete ((CCoord*)m_pMapObject);
   }
   else if (m_nDataType == BDIMAGE)
   {
      delete ((CImageFile*)m_pMapObject);
   }
   m_pMapObject = NULL;
};

/////////////////////////////////////////////////////////////////////////////

CMapLayer::CMapLayer(BOOL bMapLines)
{
   // Initialise variables   

   m_lFType = 0;   
   m_pQuery = NULL;   
   m_bExtentChecked = FALSE;
   m_bOnLoad = FALSE;
   
   // Set best fit as the default for map lines

   if (bMapLines) m_prop.m_nTextPos = BestFit;

   
}

/////////////////////////////////////////////////////////////////////////////

CMapLayer::CMapLayer(CMapLayer& rSrc)
{
   m_lFType = rSrc.m_lFType;   
   m_aAttr.Copy(rSrc.m_aAttr);
   m_prop = rSrc.m_prop;   
   if (rSrc.m_pQuery == NULL) m_pQuery = NULL;
   else m_pQuery = new CQuery(*rSrc.m_pQuery);   
   m_alFeatures.Copy(rSrc.m_alFeatures);   
 
   m_bExtentChecked = FALSE;
   m_bOnLoad = FALSE;


}

/////////////////////////////////////////////////////////////////////////////

CMapStyle::CMapStyle()
{
   m_crLine = RGB(0,0,0);
   m_crFill = RGB(64,64,64);
   m_nPattern = BS_SOLID;
   m_nHatch = 0;
   m_nLineStyle = PS_SOLID;   
   m_nLineWidth = 1;
   m_nSymbol = CComboBoxSymbol::circle;   
   m_dSymSize = 1;            
}

CMapProperties::CMapProperties()
{      
   m_colorText = RGB(0,0,0);      
   m_bOverlap = FALSE;
   m_bScaleFont = FALSE;
   m_nTextPos = CMapLayer::BestFit;
   m_bBestFitSym = FALSE;   
   m_bShowLayerName = TRUE;
   m_bVisible = TRUE;
   m_nSepColour = 0;
   m_bRangeColour = FALSE;
   m_bAutoColour = FALSE;
   m_bAutoSize = FALSE;
   m_bPolygon = TRUE;
   m_dAutoMin = 0;
   m_dAutoMax = 0;

   memset(&m_logfont,0,sizeof(LOGFONT));
   m_logfont.lfHeight = -10;
   m_logfont.lfPitchAndFamily = 18;
   m_logfont.lfCharSet = NRDB_CHARSET;
   strcpy(m_logfont.lfFaceName, BDString(IDS_DEFAULTFONT));      

   memset(&m_logfontC,0,sizeof(LOGFONT));
   m_logfontC.lfHeight = -8;
   m_logfontC.lfPitchAndFamily = 12;   
   m_logfontC.lfCharSet = NRDB_CHARSET;
   strcpy(m_logfontC.lfFaceName, BDString(IDS_DEFAULTFONT));         
}

/////////////////////////////////////////////////////////////////////////////
// Copy the map property attributes
//

CMapStyle::CMapStyle(CMapStyle& rSrc)
{
   *this = rSrc;
}


CMapStyle& CMapStyle::operator=(const CMapStyle& rSrc)
{
   m_crLine = rSrc.m_crLine;
   m_crFill = rSrc.m_crFill;
   m_nPattern = rSrc.m_nPattern;
   m_nHatch = rSrc.m_nHatch;
   m_nLineStyle = rSrc.m_nLineStyle;
   m_nLineWidth = rSrc.m_nLineWidth;
   m_nSymbol = rSrc.m_nSymbol;
   m_dSymSize = rSrc.m_dSymSize;   
   
   return *this;
}

BOOL CMapStyle::operator==(const CMapStyle& rSrc)
{
   return m_crLine == rSrc.m_crLine &&
   m_crFill == rSrc.m_crFill &&
   m_nPattern == rSrc.m_nPattern &&
   m_nHatch == rSrc.m_nHatch &&
   m_nLineStyle == rSrc.m_nLineStyle &&
   m_nLineWidth == rSrc.m_nLineWidth &&
   m_nSymbol == rSrc.m_nSymbol &&
   m_dSymSize == rSrc.m_dSymSize;   
}

///////////////////////////////////////////////////////////////////////////////

void CMapStyle::AsXML(CXMLObj* pXMLStyle)
{
   CXMLObj *pXMLChild = NULL;

   pXMLStyle->m_sName = "style";

   // Brush

   pXMLChild = pXMLStyle->AddChild("brushstyle");
   pXMLChild->SetValue("", "pattern", m_nPattern);
   pXMLChild->SetValue("", "hatch", m_nHatch);
   pXMLChild->SetValue("", "colour", (long)m_crFill);

   // Line

   pXMLChild = pXMLStyle->AddChild("linestyle");
   pXMLChild->SetValue("", "line", m_nLineStyle);
   pXMLChild->SetValue("", "width", m_nLineWidth);
   pXMLChild->SetValue("", "colour", (long)m_crLine);

   // Symbol

   pXMLChild = pXMLStyle->AddChild("symbolstyle");
   pXMLChild->SetValue("", "symbol", m_nSymbol);
   pXMLChild->SetValue("", "size", m_dSymSize);
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts an xml object to a map style object
//

BOOL CMapStyle::XMLAs(CXMLObj* pXMLObj)
{
   BOOL bOK = TRUE;

   if (pXMLObj != NULL && pXMLObj->m_sName == "style") pXMLObj = pXMLObj->GetChild();
   if (pXMLObj != NULL)
   {
      m_nLineStyle = pXMLObj->GetInteger("linestyle", "line");
      m_nLineWidth = pXMLObj->GetInteger("linestyle", "width");
      m_crLine = pXMLObj->GetInteger("linestyle", "colour");
      m_crFill = pXMLObj->GetInteger("brushstyle", "colour");
      m_nPattern = pXMLObj->GetInteger("brushstyle", "pattern");
      m_nHatch = pXMLObj->GetInteger("brushstyle", "hatch");
      m_nSymbol = pXMLObj->GetInteger("symbolstyle", "symbol");
      m_dSymSize = pXMLObj->GetDouble("symbolstyle", "size");     
   } else
   {
      bOK = FALSE;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

CString CMapStyle::AsString()
{
   CString s;

   s.Format("%li,%li,%li,%li,%li,%li,%li,%lf", m_crFill, m_crLine, m_nLineStyle, 
            m_nLineWidth, m_nPattern, m_nHatch, m_nSymbol, m_dSymSize);
   return s;

}

BOOL CMapStyle::StringAs(CString s)
{
   if (sscanf(s, "%li, %li,%li,%li,%li,%li,%li,%lf", &m_crFill, &m_crLine, &m_nLineStyle, 
            &m_nLineWidth, &m_nPattern, &m_nHatch, &m_nSymbol, &m_dSymSize) == 8)
   {
      // Skip passed 

      int i = 0; for (i = 0; i < 8; i++)
      {
         int j = s.Find(',');
         s = s.Mid(j+1);
      }      

      return TRUE;
   }
   return FALSE;
};

/////////////////////////////////////////////////////////////////////////////

BOOL CStyleSchemes::XMLAs(CXMLObj* pXMLObj)
{   
   CMapStyle style;
   BOOL bOK = TRUE;
   CXMLObj *pXMLChild;

   RemoveAll();

   BOOL bFound = pXMLObj->GetFirst("styleschemes", "stylescheme", pXMLChild);
   while (bFound && bOK)
   {
      CStyleScheme scheme;
      scheme.m_sName = pXMLChild->GetAttr("name");

      CXMLObj *pXMLStyle = pXMLChild->GetChild();
      while (pXMLStyle != NULL && bOK)
      {
         if (pXMLStyle->m_sName == "style")
         {
            if (style.XMLAs(pXMLStyle->GetChild()))
            {
               scheme.m_aStyle.Add(style);
            } else
            {
               bOK = FALSE;
            }
         }
         pXMLStyle = pXMLStyle->GetNext();
      }
      Add(scheme);

      bFound = pXMLObj->GetNext("styleschemes", "stylescheme", pXMLChild);
   }
   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void CStyleSchemes::AsXML(CXMLObj& xmlobj)
{
   CString s;

   CXMLObj *pXMLObj = &xmlobj;
   xmlobj.m_sName = "styleschemes";

   int i = 0; for (i = 0; i < GetSize(); i++)  
   {
      if (!GetAt(i).m_bSystem)
      {         
         pXMLObj = new CXMLObj;
         xmlobj.AddChild(pXMLObj);

         pXMLObj->m_sName = "stylescheme";
         pXMLObj->SetAttr("name", GetAt(i).m_sName);
         int j = 0; for (j = 0; j < GetAt(i).m_aStyle.GetSize(); j++)
         {
            CXMLObj *pXMLChild = new CXMLObj;
            pXMLChild->m_sName = "style";
            GetAt(i).m_aStyle[j].AsXML(pXMLChild);

            s.Format("%d",j);
            pXMLChild->SetAttr("id", s);

            pXMLObj->AddChild(pXMLChild);
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Convert an XML object to an array of colour schemes
//

BOOL CRangeSchemes::XMLAs(CXMLObj* pXMLObj)
{
   BOOL bOK = TRUE;
   CXMLObj *pXMLChild;
   CColourRangeArray aRangeColour;

   RemoveAll();

   // Retrieve the colour range schemes from the XML object and add to the array

   BOOL bFound = pXMLObj->GetFirst("styleschemes", "colourranges", pXMLChild);
   while (bFound && bOK)
   {
      aRangeColour.m_sName = pXMLChild->GetAttr("name");
      aRangeColour.m_dRound = pXMLChild->GetDouble("", "round");
      aRangeColour.m_bApplyPercent = pXMLChild->GetInteger("","applypercent"); 
      if (!aRangeColour.XMLAs(pXMLChild)) bOK = FALSE;
      
      Add(aRangeColour);
      aRangeColour = CColourRangeArray();
      bFound = pXMLObj->GetNext("styleschemes","colourranges", pXMLChild);
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CColourRangeArray::XMLAs(CXMLObj *pXMLObj)
{
   BOOL bOK = TRUE;
   CColourRange colourrange;

   CXMLObj *pXMLChild, *pXMLStyle;

   // Retrieve the array of colour ranges each containing a maximum and minimum value and a style

   BOOL bFound = pXMLObj->GetFirst("colourranges", "colourrange", pXMLChild);
   while (bFound && bOK)
   {
      colourrange.m_dMin = pXMLChild->GetDouble("","min");
      colourrange.m_dMax = pXMLChild->GetDouble("","max");

      pXMLStyle = pXMLChild->GetXMLObj("", "style");
      if (pXMLStyle != NULL)
      {
         if (!colourrange.XMLAs(pXMLStyle)) bOK = FALSE;
         Add(colourrange);
      } else
      {
         bOK = FALSE;
      }

      bFound = pXMLObj->GetNext("colourranges", "colourrange", pXMLChild);
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void CRangeSchemes::AsXML(CXMLObj& xmlobj)
{
   CString s;

   CXMLObj *pXMLObj = &xmlobj;

   xmlobj.m_sName = "styleschemes";

   int i = 0; for (i = 0; i < GetSize(); i++)  
   {
      pXMLObj = xmlobj.AddChild("colourrange");
      GetAt(i).AsXML(pXMLObj);
      pXMLObj->SetAttr("name", GetAt(i).GetName());    
   }
}

///////////////////////////////////////////////////////////////////////////////

void CColourRangeArray::AsXML(CXMLObj * pXMLObj)
{
   pXMLObj->m_sName = "colourranges";
   
   pXMLObj->SetValue("", "round", m_dRound);
   pXMLObj->SetValue("", "applypercent", m_bApplyPercent);

   int j = 0; for (j = 0; j < GetSize(); j++)
   {
      CXMLObj *pXMLChild = pXMLObj->AddChild("colourrange");
      pXMLChild->SetValue("","min", GetAt(j).m_dMin);
      pXMLChild->SetValue("","max", GetAt(j).m_dMax);

      CXMLObj *pXMLStyle = pXMLChild->AddChild("style");
      GetAt(j).AsXML(pXMLStyle);

   }
}

///////////////////////////////////////////////////////////////////////////////

CMapGrid::CMapGrid()
{
   memset(&m_logfont,0,sizeof(LOGFONT));
   m_logfont.lfHeight = -8;
   m_logfont.lfPitchAndFamily = 18;
   m_logfont.lfCharSet = NRDB_CHARSET;
   strcpy(m_logfont.lfFaceName, BDString(IDS_DEFAULTFONT));      

   m_nType = defaultgrid;
   m_nDegLat = 1;
   m_nMinLat = 0;
   m_nSecLat = 0;
   m_nDegLng = 1;
   m_nMinLng = 0;
   m_nSecLng = 0;

   m_style.m_crLine = RGB(166,202,240);
}

///////////////////////////////////////////////////////////////////////////////

CMapGrid::CMapGrid(CMapGrid& rSrc)
{
   m_logfont = rSrc.m_logfont;
   m_style = rSrc.m_style;
   m_nType = rSrc.m_nType;
   m_nDegLat = rSrc.m_nDegLat;
   m_nMinLat = rSrc.m_nMinLat;
   m_nSecLat = rSrc.m_nSecLat;
   m_nDegLng = rSrc.m_nDegLng;
   m_nMinLng = rSrc.m_nMinLng;
   m_nSecLng = rSrc.m_nSecLng;
}

///////////////////////////////////////////////////////////////////////////////

CMapProperties::CMapProperties(CMapProperties& rSrc)
{      
   *this = rSrc;
}

CMapProperties& CMapProperties::operator=(CMapProperties& rSrc)
{
   m_sName = rSrc.m_sName;      
   m_colorText = rSrc.m_colorText;      
   m_bOverlap = rSrc.m_bOverlap;
   m_nTextPos = rSrc.m_nTextPos;
   m_sComment = rSrc.m_sComment;
   m_bAutoColour = rSrc.m_bAutoColour;
   m_bAutoSize = rSrc.m_bAutoSize;
   m_bPolygon = rSrc.m_bPolygon;
   m_bScaleFont = rSrc.m_bScaleFont;
   m_bBestFitSym = rSrc.m_bBestFitSym;
   m_bShowLayerName = rSrc.m_bShowLayerName;
   m_bVisible = rSrc.m_bVisible;
   m_nSepColour = rSrc.m_nSepColour;
   m_bRangeColour = rSrc.m_bRangeColour;
   m_dAutoMin = rSrc.m_dAutoMin;
   m_dAutoMax = rSrc.m_dAutoMax;
   m_logfont = rSrc.m_logfont;   
   m_logfontC = rSrc.m_logfontC;
   m_aColourFeature.Copy(rSrc.m_aColourFeature);
   m_aRangeColour = rSrc.m_aRangeColour;

   (CMapStyle&)*this = (CMapStyle&)rSrc;

   return *this;
}

/////////////////////////////////////////////////////////////////////////////

CMapLayoutObj::CMapLayoutObj()
{
   m_rect = CRect(-1,-1,-1,-1);   
   memset(&m_logfont,0,sizeof(LOGFONT));
   m_logfont.lfHeight = -12;
   m_logfont.lfPitchAndFamily = 18;
   m_logfont.lfCharSet = NRDB_CHARSET;
   m_nType = CMapLayout::none;   
}

CMapLayoutObj& CMapLayoutObj::operator=(const class CMapLayoutObj& rSrc)
{
   m_rect = rSrc.m_rect;
   m_style = rSrc.m_style;
   m_logfont = rSrc.m_logfont;
   m_nType = rSrc.m_nType;
   m_sImageFile = rSrc.m_sImageFile;
   m_sText = rSrc.m_sText;

   return *this;
}

CMapLayoutObj::CMapLayoutObj(const CMapLayoutObj& rSrc)
{
   *this = rSrc;
}

/////////////////////////////////////////////////////////////////////////////

CString CMapLayoutObj::GetTypeDesc()
{
   if (m_nType == CMapLayout::map) return BDString(IDS_MAP);
   if (m_nType == CMapLayout::legend) return BDString(IDS_LEGEND);
   if (m_nType == CMapLayout::title) return BDString(IDS_TITLE);
   if (m_nType == CMapLayout::arrow) return BDString(IDS_NORTHARROW);
   if (m_nType == CMapLayout::source) return BDString(IDS_COMMENTS);
   if (m_nType == CMapLayout::scalebar) return BDString(IDS_SCALEBAR);
   if (m_nType == CMapLayout::projection) return BDString(IDS_PROJECTION);
   if (m_nType == CMapLayout::box) return BDString(IDS_BORDER);
   if (m_nType == CMapLayout::picture) return BDString(IDS_IMAGE);
   if (m_nType == CMapLayout::text) return BDString(IDS_TEXT);   
   if (m_nType == CMapLayout::scale) return BDString(IDS_SCALE);   
   if (m_nType == CMapLayout::locator) return BDString(IDS_LOCATOR);

   ASSERT(FALSE);
   return "";      
}

/////////////////////////////////////////////////////////////////////////////

CMapLayout::CMapLayout()
{
   CMapLayoutObj layoutobj;

   layoutobj.m_nType = map;
   layoutobj.m_style.m_crLine = RGB(0,0,64);
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = legend;
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = title;
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = arrow;
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = source;
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = scalebar;
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = box;
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = projection;
   m_aLayout.Add(layoutobj);

   layoutobj.m_nType = location;
   m_aLayout.Add(layoutobj);

   m_bDefault = FALSE;
}

CMapLayout& CMapLayout::operator=(CMapLayout& rSrc)
{
   m_sName = rSrc.m_sName;
   m_bDefault = rSrc.m_bDefault;
   m_aLayout.Copy(rSrc.m_aLayout);
   return *this;
}

CMapLayout::CMapLayout(CMapLayout& rSrc)
{
   *this = rSrc;
}

CMapLayoutObj CMapLayout::GetLayoutObj(int nType)
{   
   int i = 0; for (i = 0; i < m_aLayout.GetSize(); i++)
   {
      if (m_aLayout[i].m_nType == nType) return m_aLayout[i]; 
   }
   
   return CMapLayoutObj();
}

CMapLayoutObj* CMapLayout::GetLayoutObjPtr(int nType)
{
   int i = 0; for (i = 0; i < m_aLayout.GetSize(); i++)
   {
      if (m_aLayout[i].m_nType == nType) return &m_aLayout[i]; 
   }
   
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////

void CMapLayoutObj::AsXML(CXMLObj* pXML)
{
   pXML->m_sName = "layoutobj";

   // Rectangle
   CXMLObj *pXMLObj = new CXMLObj;
   pXMLObj->m_sName = "rect";
   pXMLObj->m_sValue.Format("%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
   pXML->AddChild(pXMLObj);

   // style
   pXMLObj = new CXMLObj;
   pXMLObj->m_sName = "style";
   m_style.AsXML(pXMLObj);
   pXML->AddChild(pXMLObj);

   // logfont
   pXMLObj = new CXMLObj;
   pXMLObj->m_sName = "logfont";
   pXMLObj->m_sValue = AsString(&m_logfont);
   pXML->AddChild(pXMLObj);

   // Type
   pXMLObj = new CXMLObj;
   pXMLObj->m_sName = "type";   
   pXMLObj->m_sValue.Format("%d", m_nType);
   pXML->AddChild(pXMLObj);

   // Imagefile
   pXMLObj = new CXMLObj;
   pXMLObj->m_sName = "imagefile";   
   pXMLObj->m_sValue = m_sImageFile;
   pXML->AddChild(pXMLObj);

   // Text
   pXMLObj = new CXMLObj;
   pXMLObj->m_sName = "text";   
   pXMLObj->m_sValue = m_sText;
   pXML->AddChild(pXMLObj);

}

/////////////////////////////////////////////////////////////////////////////

BOOL CMapLayoutObj::XMLAs(CXMLObj* pXMLObj)
{
   BOOL bOK = TRUE;

   // Get rectangle

   bOK = sscanf(pXMLObj->GetString("layoutobj", "rect"), "%d,%d,%d,%d", 
          &m_rect.left, &m_rect.top, &m_rect.right, &m_rect.bottom) == 4;
   
   // Get style
   bOK &= m_style.XMLAs(pXMLObj->GetXMLObj("layoutobj", "style"));
         
   // Get font
   bOK &= StringAs(pXMLObj->GetString("layoutobj", "logfont"), &m_logfont);

   // Get type
   m_nType = pXMLObj->GetInteger("layoutobj", "type");

   // Get image file
   m_sImageFile = pXMLObj->GetString("layoutobj", "imagefile");

   // Get text
   m_sText = pXMLObj->GetString("layoutobj", "text");


   // If type box and layout version < 2.01 then set style to transparent

   if (m_nType == CMapLayout::box && CMapLayoutArray::m_dLayoutVersion < 2.01)
   {
      m_style.m_nPattern = BS_NULL;
   }

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

void CMapLayout::SetLayoutObj(int nType, CMapLayoutObj layoutobj)
{

   int i = 0; for (i = 0; i < m_aLayout.GetSize(); i++)
   {
      if (m_aLayout[i].m_nType == nType)
      {
         m_aLayout[i] = layoutobj;
         break;
      };
   }
}


/////////////////////////////////////////////////////////////////////////////
//
// Returns a rectangle containing the percentage of the second rectangle of the
// first
//

CRect CMapLayout::RectAsPercent(CRect rectT, CRect rect)
{
   CRect rectP;
   rectP.left = int(((rect.left-rectT.left)*1000)  / (double)rectT.Width() + 0.5);
   rectP.right = int(((rect.right-rectT.left)*1000)  / (double)rectT.Width() + 0.5);
   rectP.top = int(((rect.top-rectT.top)*1000)  / (double)rectT.Height() + 0.5);      
   rectP.bottom = int(((rect.bottom-rectT.top)*1000)  / (double)rectT.Height() + 0.5);      

   return rectP;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns a rectangle given the percentage of the the first rectangle in the
// second
//

CRect CMapLayout::RectFromPercent(CRect rectT, CRect rectP)
{
   CRect rect;
   rect.left = int(rectT.left + (rectT.Width() * rectP.left)/1000.0 + 0.5);
   rect.right = int(rectT.left + (rectT.Width() * rectP.right)/1000.0 + 0.5);
   rect.top = int(rectT.top + (rectT.Height() * rectP.top)/1000.0 + 0.5);
   rect.bottom = int(rectT.top + (rectT.Height() * rectP.bottom)/1000.0 + 0.5);

   return rect;
}

/////////////////////////////////////////////////////////////////////////////

void CMapLayout::AsXML(CXMLObj* pXMLObj)
{
   pXMLObj->m_sName = "layout";

   // Add name of layout

   pXMLObj->SetAttr("name", m_sName);
   pXMLObj->SetAttr("default", m_bDefault);

   // Add array of objects

   int i = 0; for (i = 0; i < m_aLayout.GetSize(); i++)
   {      
      CXMLObj *pXMLObj = new CXMLObj;
      m_aLayout[i].AsXML(pXMLObj);

      pXMLObj->AddChild(pXMLObj);
   }
   
}

/////////////////////////////////////////////////////////////////////////////

BOOL CMapLayout::XMLAs(CXMLObj* pXMLObj)
{
   BOOL bOK = TRUE;
   

   m_aLayout.RemoveAll();

   // Retrieve the attributes

   m_sName = pXMLObj->GetAttr("name");
   sscanf(pXMLObj->GetAttr("default"), "%d", &m_bDefault);   

   CXMLObj *pXMLChild = NULL;
   while (bOK && pXMLObj->GetChild("layoutobj", pXMLChild))
   { 
      CMapLayoutObj layoutobj;
      bOK = layoutobj.XMLAs(pXMLChild);

      m_aLayout.Add(layoutobj);
   };   
       

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

void CMapLayoutArray::AsXML(CXMLObj* pXMLObj)
{  
   pXMLObj->m_sName = "layoutarray";

   pXMLObj->SetAttr("version", "2.01");
   
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      CXMLObj *pXMLObj = new CXMLObj;
      GetAt(i).AsXML(pXMLObj);

      pXMLObj->AddChild(pXMLObj);
   }
}

///////////////////////////////////////////////////////////////////////////////

BOOL CMapLayoutArray::XMLAs(CXMLObj* pXML)
{
   BOOL bOK = TRUE;

   RemoveAll();

   m_dLayoutVersion = 0;
   sscanf(pXML->GetAttr("version"), "%lf", &m_dLayoutVersion);   

   // Read through layouts

   CXMLObj *pXMLObj = NULL; // Must be null
   while (bOK && pXML->GetChild("layout", pXMLObj))
   {     
       CMapLayout layout;              
       bOK = layout.XMLAs(pXMLObj);

       // Add to array

       Add(layout);
   }
   

   return bOK; 
}

/////////////////////////////////////////////////////////////////////////////
//
// Return the default map layout
//

CMapLayout CMapLayoutArray::GetDefault()
{
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      if (GetAt(i).m_bDefault) return GetAt(i);
   }

   if (GetSize() > 0) 
   {
      // Set this as default
      CMapLayout layout = GetAt(0);
      layout.m_bDefault = TRUE;
      SetAt(0, layout);

      return GetAt(0);
   }
   else return CMapLayout();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CMapLayer::Initialise(long lFType, CLongArray& alFeatures, CFTypeAttrArray& aAttr, COLORREF color)
{   
   BOOL bOK = TRUE;   

   m_lFType = lFType;   
      
   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)    
   {
      CQueryAttrSel attrsel;
      attrsel.m_lAttr = aAttr[i].m_lAttrId;
      attrsel.m_lFType = aAttr[i].m_lFType;
      attrsel.m_lDataType = 0;
      m_aAttr.Add(attrsel);      
   };

   // Create the map layer objects

   m_alFeatures.Copy(alFeatures);
   
   // Load the data

   if (bOK)
   {
      bOK = LoadData();   
   };

   // Copy the selected items into the object.  This is done after the
   // data is loaded so that it is known whether the layer contains polylines
   // or polygons
   
   if (m_prop.m_bPolygon) m_prop.m_crFill = color;
   else m_prop.m_crLine = color;      
   m_prop.m_colorText = RGB(0,0,0);

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

void CMapLayer::RemoveAll()
{
   int i = 0; for (i = 0; i < GetSize(); i++)
   {        
	  CMapLayerObj* pMapLayerObj = (CMapLayerObj*)GetAt(i);
     if (pMapLayerObj != NULL) delete pMapLayerObj;
     SetAt(i, NULL);
   }
   CArray <CMapLayerObj*,  CMapLayerObj*>::RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////

CMapLayer::~CMapLayer()
{ 
   if (m_pQuery != NULL) 
   {
      delete m_pQuery;
      m_pQuery = NULL;
   }

   RemoveAll();
}



///////////////////////////////////////////////////////////////////////////////

CMapLayerArray::CMapLayerArray()
{   
   m_iDefault = 0;

   Reset();
   
   // Initialise static variables
   
   memset(&m_logfontT,0,sizeof(LOGFONT));
   m_logfontT.lfHeight = -16;
   m_logfontT.lfPitchAndFamily = 18;
   m_logfontT.lfCharSet = NRDB_CHARSET;
   
   m_logfontT.lfWeight = 700;

   memset(&m_logfontL,0,sizeof(LOGFONT));
   m_logfontL.lfHeight = -10;
   m_logfontL.lfPitchAndFamily = 18;
   m_logfontL.lfCharSet = NRDB_CHARSET;
   
   m_logfontL.lfWeight = 700;

   m_bLegend1Font = FALSE;
	
   // Don't initialise report wizard object as program loads

   if (afxCurrentResourceHandle == NULL) return;

   strcpy(m_logfontL .lfFaceName, BDString(IDS_DEFAULTFONT));      
   strcpy(m_logfontT.lfFaceName, BDString(IDS_DEFAULTFONT));      

   m_sTitle = BDGetSettings().m_Organization;
   
}

/////////////////////////////////////////////////////////////////////////////

void CMapLayerArray::Reset()
{
   m_layout = CMapLayout();
}

/////////////////////////////////////////////////////////////////////////////

CMapLayerArray::~CMapLayerArray()
{
   RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
//
// Copy attributes but not array contents
//


CMapLayerArray& CMapLayerArray::operator=(CMapLayerArray& rSrc)
{   
   m_layout = rSrc.m_layout;

   m_sTitle = rSrc.m_sTitle;
   m_logfontT = rSrc.m_logfontT;
   m_logfontL = rSrc.m_logfontL;
   m_bLegend1Font = rSrc.m_bLegend1Font;
 
   m_mapgrid = rSrc.m_mapgrid;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void CMapLayerArray::RemoveAll()
{
   int i = 0; for (i = 0; i < GetSize(); i++)
   {            
      delete (CMapLayer*)GetAt(i);
   }
   CArray <CMapLayer*, CMapLayer*>::RemoveAll();
}


///////////////////////////////////////////////////////////////////////////////

BOOL CMapLayer::AddAttribute(CMapLayerObj* pMapLayerObj, CAttrArray& aAttr)
{  
   BOOL bOK = TRUE;
   CString sAttrName;
   double dValue = AFX_RFX_DOUBLE_PSEUDO_NULL;
   BOOL bFound = FALSE;

  // Determine name / attributes

   if (m_pQuery != NULL)
   {
      sAttrName = m_aAttr.GetAttrDesc(aAttr);
   }

   // Determine the value of the attribute

   int i = 0; for (i = 0; i < m_aAttr.GetSize(); i++)
   {
      int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
      {
         if (m_aAttr[i].m_lAttr == aAttr.GetAt(j)->GetAttrId() &&
             aAttr[j]->GetDataType() == BDNUMBER)      
         {
             dValue = *aAttr[j]->GetDouble();             
         }
      };
   };

   // Determine the map objects to display

   for (i = 0; i < m_aAttr.GetSize() && bOK; i++)
   {
      int j = 0; for (j = 0; j < aAttr.GetSize() && bOK; j++)
      {
         if (m_aAttr[i].m_lAttr == aAttr.GetAt(j)->GetAttrId())      
         {
            CAttribute* pAttr = aAttr[j];
            if (pAttr->GetFTypeId() == m_aAttr[i].m_lFType)
            {
               // Set the text associated with the object

               pMapLayerObj->SetText(sAttrName);

               // Set the date associated with the object

               pMapLayerObj->SetDate(aAttr.m_lDate);

               if (pAttr->GetDataType() == BDMAPLINES)
               {                        

                  CSpatialPoly *pSpatialPoly = new CSpatialPoly(*pAttr->GetLongBinary());
				  
                  // Check if polygon                  
                  

                  if (!pSpatialPoly->m_bPolygon && !m_bOnLoad)
                  {
                     m_prop.m_nPattern = BS_NULL;
                     m_prop.m_crLine = -1;
                     m_prop.m_bPolygon = FALSE;

                  }
              
                  if (pSpatialPoly != NULL)
                  {
                     pMapLayerObj->SetMapObject(pSpatialPoly);                        
                     pMapLayerObj->SetDataType(BDMAPLINES);

                     if (!IsNullDouble(dValue))
                     {
                        pMapLayerObj->SetValue(dValue);
                     }
                     bFound = TRUE;
                  } else
                  {
                     bOK = FALSE;
                     AfxMessageBox(BDString(IDS_OUTOFMEMORY));
                  }
                  break;
               }

               // Add coordinates to the map

               else if (pAttr->GetDataType() == BDCOORD)
               {	     
                  // Set polygon to true for symbols so that custom
                  // fills can be used

                  if (!m_bOnLoad) m_prop.m_bPolygon = TRUE;

                  CCoord* pCoord = new CCoord(*pAttr->GetCoord());
                  if (pCoord != NULL)
                  {
                     if (!pCoord->IsNull())
                     {
                        pMapLayerObj->SetMapObject(pCoord);                 
                        if (!IsNullDouble(dValue))
                        {
                           pMapLayerObj->SetValue(dValue);
                        }
                        bFound = TRUE;
                        pMapLayerObj->SetDataType(BDCOORD);                     
                     };
                  } else
                  {
                     bOK = FALSE;
                     AfxMessageBox(BDString(IDS_OUTOFMEMORY));
                  }
                  break;
               }  
               
               // Image

               else if (pAttr->GetDataType() == BDIMAGE)
               {
                  if (!m_bOnLoad) m_prop.m_bPolygon = FALSE;
                  
                  CImageFile* pImage = new CImageFile;

                  m_prop.m_nSymbol = CComboBoxSymbol::none;
                  
                  if (pImage != NULL)
                  {
                     pImage->Initialise(*pAttr->GetLongBinary());                 
                     
                     pMapLayerObj->SetMapObject(pImage);                 
                     if (!IsNullDouble(dValue))
                     {
                        pMapLayerObj->SetValue(dValue);
                     }
                     bFound = TRUE;
                     pMapLayerObj->SetDataType(BDIMAGE);                                                               
                  } else
                  {
                     bOK = FALSE;
                     AfxMessageBox(BDString(IDS_OUTOFMEMORY));
                  }
                  break;
               }

 
            };

         };
      };     
   };   

   // If not found then error (e.g. if produced using a query statistic)

   if (!bFound) bOK = FALSE;
   
   return bOK;
}


/////////////////////////////////////////////////////////////////////////////
//
// Determines whether feature type name and parent feature type name are 
// required, also determines the fastest way of querying the data - query
// each datum required or retrieve all data and search
//

BOOL CMapLayer::LoadData(BOOL bOnLoad)
{
   // Create query

   m_bOnLoad = bOnLoad;

   // Load data from overlay file

   if (m_sFile != "")
   {
      if (CImageFile::IsImageFile(m_sFile))         
      {
         // Indicate to load georeferences, only run utility to edit if not
         // loading from .nrm file

         int nFlag = CImageFile::GeoRef;
         if (bOnLoad) nFlag |= CImageFile::OnLoad;
                 
         CImageFile* pImage = new CImageFile;
         pImage->Open(m_sFile, nFlag);

         CMapLayerObj* pMapLayerObj = new CMapLayerObj;
         
         pMapLayerObj->SetMapObject(pImage); 
         pMapLayerObj->SetDataType(BDIMAGE);
         

         Add(pMapLayerObj);


      } else
      {
         CDlgAddOverlay dlg(this, m_sFile, m_sColumn);
	      return dlg.DoModal() == IDOK;
      };
   }

   // Create query from selections (probably no longer needed!)

   else
   {
	   if (m_pQuery == NULL)
	   {        
		  m_pQuery = new CQuery(m_lFType, m_aAttr, m_alFeatures);
	   }
	   // Queries

	   if (m_pQuery != NULL)
	   {
		  return Initialise(m_pQuery);
	   } 
   };

   m_bOnLoad = FALSE;

   return FALSE;   
};


///////////////////////////////////////////////////////////////////////////////
//
// Update an existing map object
//

void CMapLayerArray::Update(CMapLayer* pMapLayer, CMapLayerObj* pMapLayerObj)
{        
   CAttrArray aAttr;
   CString sFeature, sParentFeature;

   pMapLayerObj->FreeMem();

   //BDGetApp()->SetProgressBar(NULL);
   
   aAttr.m_lFeature = pMapLayerObj->GetFeature();
   aAttr.m_lFType = pMapLayer->GetFType();
   
   // Retrieve feature

   CFeature feature;
   feature.m_lId = pMapLayerObj->GetFeature();
   feature.m_lFeatureTypeId = pMapLayer->GetFType();
   BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT);
   BDEnd(BDHandle());
   
   // Requery the data for the one feature
   
	CMapLayer maplayer;	   

   // Handle queries

	if (pMapLayer->GetQuery() != NULL)
	{
		// Create a copy of the original query with only the updated feature

      CQuery* pQuery = new CQuery; // Delete in CMapLayer destructor
		*pQuery = *pMapLayer->GetQuery();
		CQueryElement* pElement = pQuery;
		while (pElement != NULL)
		{
		  if (pElement->GetDataType() == BDFEATURE)
		  {
			 CQueryLink* pLink = (CQueryLink*)pElement;
			 CQueryFeature qf = feature;
			 qf.SetSelected();
			 pLink->m_aFeatures.RemoveAll();			 				 
			 pLink->m_aFeatures.Add(qf);
			 break;
		  }
		  pElement = pElement->GetNextQuery();
		}
		maplayer.Initialise(pQuery);       
	} 
	// Add data to existing layer

	int i = 0; for (i = 0; i < maplayer.GetSize(); i++)
	{	  
	  pMapLayer->Add(maplayer.GetAt(i));   
	}

   // Prevents deletion in destructor

	maplayer.CArray <CMapLayerObj*,  CMapLayerObj*>::RemoveAll(); 

   // Need to add the feature to the existing query so that it is saved
   // with the layer

   CQueryElement* pElement = pMapLayer->GetQuery();
	while (pElement != NULL)
   {
	   if (pElement->GetDataType() == BDFEATURE)
      {
          CQueryLink* pLink = (CQueryLink*)pElement;
			 CQueryFeature qf = feature;
			 qf.SetSelected();			 
			 pLink->m_aFeatures.Add(qf);
			 break;
      };	
      pElement = pElement->GetNextQuery();      
   };
   
   // Tidy up, delete the map layer object
      
   for (i = 0; i < pMapLayer->GetSize(); i++)
   {
      if (pMapLayerObj == pMapLayer->GetAt(i))
      {
         pMapLayer->RemoveAt(i);
         delete pMapLayerObj;
         break;
      }
   }      
}


///////////////////////////////////////////////////////////////////////////////

void CMapLayerArray::Write(LPCSTR sFile)
{
   CString s;

   CXMLFile xmlfile;
   AsXML(&xmlfile);
   if (!xmlfile.Write(sFile))
   {
      AfxMessageBox(IDS_ERRORWRITE);
   }

   /*FILE* pFile = fopen(sFile, "w");
   if (pFile != NULL)
   {

      
	  fprintf(pFile,"DataSource=%s\n",BDGetDataSource());
    
     // Write map extent

     CViewMap* pViewMap = BDGetDocMap()->GetViewMap();
     fprintf(pFile,"Zoom=%lf\n", pViewMap->m_dZoom);
     fprintf(pFile,"OffX=%lf\n", pViewMap->m_dOffX);
     fprintf(pFile,"OffY=%lf\n", pViewMap->m_dOffY);

     fprintf(pFile,"title=\"%s\"\n",m_sTitle);
     BDWrite(pFile, &m_logfontT, "logfontT");
     BDWrite(pFile, &m_logfontL, "logfontL");
     fprintf(pFile,"legend1Font=%i\n", m_bLegend1Font);

     // Map Grid

     fprintf(pFile, "gridtype=%d\n", m_mapgrid.m_nType);
     BDWrite(pFile, &m_mapgrid.m_logfont, "logfont");
     s = m_mapgrid.m_style.AsString();
     fprintf(pFile, "mapstyle=%s\n", (LPCSTR)s);
     fprintf(pFile,"latitude=%d,%d,%d\n", m_mapgrid.m_nDegLat, m_mapgrid.m_nMinLat, m_mapgrid.m_nSecLat);
     fprintf(pFile,"longitude=%d,%d,%d\n", m_mapgrid.m_nDegLng, m_mapgrid.m_nMinLng, m_mapgrid.m_nSecLng);
          
     // Write layers

      int i = 0; for (i = 0; i < GetSize(); i++)
      {
         CMapLayer* pMapLayer = (CMapLayer*)GetAt(i);
         pMapLayer->Write(pFile);         
      }
      
      fclose(pFile);
   }  */ 
}

///////////////////////////////////////////////////////////////////////////////
//
// Convert all the information to recreate the map layers array to an XML object
// The advantage of storing as XML rather than an 'ini' file is that it is more 
// flexible to maintaining backwards compatability with structure changes
//

void CMapLayerArray::AsXML(CXMLObj *pXMLObj)
{
   pXMLObj->m_sName = "maplayers";

   // Data source

   pXMLObj->SetValue("maplayers", "datasource", BDGetDataSource());

   // Map extent

    CViewMap* pViewMap = BDGetDocMap()->GetViewMap();
    CXMLObj *pXMLChild = pXMLObj->AddChild("extent");
    pXMLChild->SetValue("", "zoom", pViewMap->m_dZoom);
    pXMLChild->SetValue("", "offx", pViewMap->m_dOffX);
    pXMLChild->SetValue("", "offy", pViewMap->m_dOffY);

    // Properties shared by all layers

    pXMLChild = pXMLObj->AddChild("legend");
    pXMLChild->SetValue("", "title", m_sTitle);
    pXMLChild->SetValue("", "fonttitle", m_logfontT);
    pXMLChild->SetValue("", "fontlegend", m_logfontL);
    pXMLChild->SetValue("", "singlefont", m_bLegend1Font);   

     // Map Grid

    pXMLChild = pXMLObj->AddChild("grid");
    pXMLChild->SetValue("", "gridtype", m_mapgrid.m_nType);
    pXMLChild->SetValue("", "font", m_mapgrid.m_logfont); 

    m_mapgrid.m_style.AsXML(pXMLChild->AddChild("style"));

    CXMLObj *pXMLLatLon = pXMLChild->AddChild("latitude");
    pXMLLatLon->SetValue("","deg",m_mapgrid.m_nDegLat);
    pXMLLatLon->SetValue("","min",m_mapgrid.m_nMinLat);
    pXMLLatLon->SetValue("","sec",m_mapgrid.m_nSecLat);
    pXMLLatLon = pXMLChild->AddChild("longitude");
    pXMLLatLon->SetValue("","deg",m_mapgrid.m_nDegLng);
    pXMLLatLon->SetValue("","min",m_mapgrid.m_nMinLng);
    pXMLLatLon->SetValue("","sec",m_mapgrid.m_nSecLng);

    // Individual map layers

    int i = 0; for (i = 0; i < GetSize(); i++)
    {
       pXMLChild = pXMLObj->AddChild("maplayer");
       CMapLayer* pMapLayer = (CMapLayer*)GetAt(i);
       pMapLayer->AsXML(pXMLChild);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts an XML object, usually loaded from a file, back into a CMapLayerArray
// including consituents, such as query, styles etc.  
//

BOOL CMapLayerArray::XMLAs(CXMLObj *pXMLObj)
{
   BOOL bOK = TRUE;

   //AfxMessageBox("Data source name");

   // Check the data source name

   CString sDataSource = pXMLObj->GetString("maplayers", "datasource");      
   if (sDataSource != BDGetDataSource())
   {
      m_sError = BDString(IDS_FILEDSN) + ": " + sDataSource;            
      return FALSE;
   }

   //AfxMessageBox("Extent");

   // Get extent

   CViewMap* pViewMap = BDGetDocMap()->GetViewMap();
   CXMLObj *pXMLChild = pXMLObj->GetXMLObj("maplayers", "extent", TRUE);
   pViewMap->m_dZoom = pXMLChild->GetDouble("", "zoom");
   pViewMap->m_dOffX = pXMLChild->GetDouble("", "offx");
   pViewMap->m_dOffY = pXMLChild->GetDouble("", "offy");

   //AfxMessageBox("Legend");

   // Get layer properties

   pXMLChild = pXMLObj->GetXMLObj("maplayers", "legend", TRUE);
   m_sTitle = pXMLChild->GetString("", "title");
   m_logfontT = pXMLChild->GetLogfont("", "fonttitle");
   m_logfontL = pXMLChild->GetLogfont("", "fontlegend");
   m_bLegend1Font = pXMLChild->GetInteger("", "singlefont");   

    // Get map grid properties

   //AfxMessageBox("Grid");

   pXMLChild = pXMLObj->GetXMLObj("maplayers", "grid", TRUE);
   m_mapgrid.m_nType = pXMLChild->GetInteger("", "gridtype");
   m_mapgrid.m_logfont = pXMLChild->GetLogfont("", "font"); 

   //AfxMessageBox("Grid Style");

   if (!m_mapgrid.m_style.XMLAs(pXMLChild->GetXMLObj("","style"))) bOK = FALSE;

   //AfxMessageBox("Grid lat/long");
     
   CXMLObj *pXMLChild1 = pXMLChild->GetXMLObj("", "latitude", TRUE);

   //AfxMessageBox("Grid lat");

   m_mapgrid.m_nDegLat = pXMLChild1->GetInteger("","deg");
   m_mapgrid.m_nMinLat = pXMLChild1->GetInteger("","min");
   m_mapgrid.m_nSecLat = pXMLChild1->GetInteger("","sec");
   pXMLChild1 = pXMLChild->GetXMLObj("", "longitude", TRUE);
   m_mapgrid.m_nDegLng = pXMLChild1->GetInteger("","deg");
   m_mapgrid.m_nMinLng = pXMLChild1->GetInteger("","min");
   m_mapgrid.m_nSecLng = pXMLChild1->GetInteger("","sec");

   //AfxMessageBox("read layers");

    // Retrieve individual map layers

   BOOL bFound = pXMLObj->GetFirst("maplayers","maplayer", pXMLChild);
   while (bFound && bOK)
   {
      // Create a new map layer and populate from XML object

       CMapLayer *pMapLayer = new CMapLayer;
       if (pMapLayer->XMLAs(pXMLChild))
       {
          // Now load data corresponding to map layer query

           if (!pMapLayer->LoadData(TRUE))
           {
              bOK = FALSE;
           }
       } else
       {
          bOK = FALSE;
       }

       // If successful add the list to the layer, otherwise delete

       if (bOK) Add(pMapLayer);
       else if (pMapLayer != NULL) delete pMapLayer;
 
       bFound = pXMLObj->GetNext("maplayers", "maplayer", pXMLChild);
   }
   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Reads data from file into structure.  
//

BOOL CMapLayerArray::Read(LPCSTR sFile)
{
   BOOL bOK = TRUE;
   int nRet, index;
   CString s;
   CString sDataSource;

   RemoveAll();

   
   AfxGetApp()->BeginWaitCursor();


   // First try to read as XML, if fails then read legacy format (NRDB Pro 2.2.2 and earlier)

   CXMLFile xmlfile;
   if (xmlfile.Read(sFile))
   {
      return XMLAs(&xmlfile);
   }
   
   // Try to read legacy format

   else
   {
      FILE* pFile = fopen(sFile, "r");

      TRY
      {      
         if (pFile != NULL)
         {
		    // Read the data source name

            sDataSource = BDNextStr(pFile);

            if (sDataSource != BDGetDataSource())
            {
               m_sError = BDString(IDS_FILEDSN) + ": " + sDataSource;            
               AfxThrowFileException(0,0,"");
            }


        // Skip layout (NRDB Pro 1 layout format)

        CRect rect;
        BDNext(pFile, rect, "rectMap");        
        BDNext(pFile, rect, "rectTitle");           
        BDNext(pFile, rect, "rectLegend");              
        BDNext(pFile, rect, "rectComments");              
        BDNext(pFile, rect, "rectNorthArrow");              
        BDNext(pFile, rect, "rectScaleBar");                   
        CDWordArray adw;
        BDNext(pFile, adw, "lineStyleMap");     
        BDNext(pFile, adw, "lineStyleLegend");     

        // Read map zoom

        CViewMap* pViewMap = BDGetDocMap()->GetViewMap();
        pViewMap->m_dZoom = BDNextDouble(pFile, "Zoom", 1);     
        pViewMap->m_dOffX = BDNextDouble(pFile, "OffX", (pViewMap->m_dMinX + pViewMap->m_dMaxX)/2);          
        pViewMap->m_dOffY = BDNextDouble(pFile, "OffY", (pViewMap->m_dMinY + pViewMap->m_dMaxY)/2);          
    
        m_sTitle = BDNextStr(pFile,"title");
        BDNext(pFile, &m_logfontT, "logfontT");
        BDNext(pFile, &m_logfontL, "logfontL");
        m_bLegend1Font = BDNextInt(pFile, "legend1Font");

        // Read Map Grid

        m_mapgrid.m_nType = BDNextInt(pFile, "gridtype", -1);
        if (m_mapgrid.m_nType != -1)
        {
           BDNext(pFile, &m_mapgrid.m_logfont, "logfont");
           BDNextStr(pFile, FALSE); 
           BDNext(pFile, m_mapgrid.m_style);
           m_mapgrid.m_nDegLat = BDNextInt(pFile, TRUE);
           m_mapgrid.m_nMinLat = BDNextInt(pFile, FALSE);
           m_mapgrid.m_nSecLat = BDNextInt(pFile, FALSE);
           m_mapgrid.m_nDegLng = BDNextInt(pFile, TRUE);
           m_mapgrid.m_nMinLng = BDNextInt(pFile, FALSE);
           m_mapgrid.m_nSecLng = BDNextInt(pFile, FALSE);
        } else
        {
           m_mapgrid = CMapGrid();
        }
     
            // For each layer read is values from the file 
            // and add it to the list
      
            do 
            {
               // Add item to list first (so it is deleted if an exception occurs)

               CMapLayer* pMapLayer = new CMapLayer;
               if (pMapLayer != NULL)
               {
                  index = Add(pMapLayer);

                  nRet = pMapLayer->Read(this, pFile);			

                  // Load data for the layer
                  if (nRet == 1)
                  {                  
                     pMapLayer->LoadData(TRUE);  
                  }
                  else if (nRet == -1)                 
                  {          
                     RemoveAt(index);
                     delete pMapLayer;
                  } else
                  {
                     bOK = FALSE;
                  }
               } else
               {
                  bOK = FALSE;
               }
            }
            while (nRet == 1 && bOK);        
         }      
      } 
      CATCH (CFileException, pEx)
      {
         bOK = FALSE;
      }
      END_CATCH

      // Delete all values on error

      if (!bOK) RemoveAll();

      // Tidy up
   
      if (pFile != NULL) fclose(pFile);

      if (BDGetProgressBar() != NULL)
      {
         BDGetProgressBar()->SetPos(0);
      };
      AfxGetApp()->EndWaitCursor();   

      return bOK;
   }
}

/////////////////////////////////////////////////////////////////////////////

/*void CMapLayer::Write(FILE* pFile)
{  
   // Output the attributes for the layer to file

   fprintf(pFile,"begin\n");         
   fprintf(pFile,"name=\"%s\"\n",m_prop.m_sName);   
   fprintf(pFile,"textcolor=%i,%i,%i\n",GetRValue(m_prop.m_colorText),GetGValue(m_prop.m_colorText),GetBValue(m_prop.m_colorText));
   fprintf(pFile,"symbol=%i,%lf\n",m_prop.m_nSymbol,m_prop.m_dSymSize);
   fprintf(pFile,"colorline=%i\n",  m_prop.m_crLine);
   fprintf(pFile,"colorfill=%i\n",  m_prop.m_crFill);
   fprintf(pFile,"pattern=%i\n",m_prop.m_nPattern);
   fprintf(pFile,"hatch=%i\n",m_prop.m_nHatch);
   fprintf(pFile,"line=%i,%i\n",m_prop.m_nLineStyle,m_prop.m_nLineWidth);
   fprintf(pFile,"text=%i,%i\n",m_prop.m_bOverlap, m_prop.m_nTextPos);   
   fprintf(pFile,"comment=\"%s\"\n",m_prop.m_sComment);
   fprintf(pFile,"fitsym=%i\n", m_prop.m_bBestFitSym);
   fprintf(pFile,"showlayername=%i\n", m_prop.m_bShowLayerName);
   fprintf(pFile,"visible=%i\n", m_prop.m_bVisible);
   fprintf(pFile,"sepcolor=%li\n",m_prop.m_nSepColour);
   if (m_prop.m_nSepColour)
   {
      m_prop.m_aColourFeature.Write(pFile, m_prop.m_nSepColour);
   }
   fprintf(pFile,"rangecolor=%i\n", m_prop.m_bRangeColour);
   if (m_prop.m_bRangeColour)
   {
      m_prop.m_aRangeColour.Write(pFile);
   }
   fprintf(pFile,"scalefont=%i\n",m_prop.m_bScaleFont);   
   fprintf(pFile,"autocolor=%i\n",m_prop.m_bAutoColour);
   fprintf(pFile,"autosize=%i\n",m_prop.m_bAutoSize);
   fprintf(pFile,"fillpolygon=%i\n",m_prop.m_bPolygon);
   fprintf(pFile,"automin=%lf\n",m_prop.m_dAutoMin);
   fprintf(pFile,"automax=%lf\n",m_prop.m_dAutoMax);
   BDWrite(pFile,&m_prop.m_logfont, "logfont");   
   BDWrite(pFile,&m_prop.m_logfontC, "logfontC");   

  // Write name of filename for overlays and column

   fprintf(pFile, "filename=%s\n", (LPCSTR)m_sFile);
   fprintf(pFile, "column=%s\n", (LPCSTR)m_sColumn);

   // Save the query

   if (m_pQuery != NULL)
   {
      fprintf(pFile, "Query\n");
      m_pQuery->Write(pFile);
   } else
   {
      fprintf(pFile, "Standard\n");
      if (m_sFile == "")
      {  
         fprintf(pFile,"ftype=%li\n",m_lFType);      
	      fprintf(pFile,"attr=");
         int i = 0; for (i = 0; i < m_aAttr.GetSize(); i++)
         {
            if (i != 0) fprintf(pFile,",");         
            fprintf(pFile,"%li",m_aAttr[i]);      
         };
      
         fprintf(pFile,"\n");

         // Output the map layer objects

         fprintf(pFile,"features=");
         for (i = 0; i < GetSize(); i++)
         {
            CMapLayerObj* pMapLayerObj = (CMapLayerObj*)GetAt(i);

            if (i > 0) fprintf(pFile,",");
            fprintf(pFile,"%li",pMapLayerObj->GetFeature());                 
         }
         fprintf(pFile,"\n");
      };
   };
   fprintf(pFile,"end\n");   
}*/

///////////////////////////////////////////////////////////////////////////////
//
//  Convert all the information to recreate the individual map layer array to an XML object

void CMapLayer::AsXML(CXMLObj *pXMLObj)
{
   CXMLObj *pXMLChild, *pXMLChild1;

   pXMLObj->SetValue("","name", m_prop.m_sName);
   pXMLObj->SetValue("","visible", m_prop.m_bVisible);
   
   // Set the style properties

   pXMLChild = pXMLObj->AddChild("style");
   m_prop.AsXML(pXMLChild);
   pXMLChild->SetValue("", "bestfitsymbol", m_prop.m_bBestFitSym);
   pXMLChild->SetValue("", "fillpolygon", m_prop.m_bPolygon);

   // Set the text properties

   pXMLChild = pXMLObj->AddChild("text");
   pXMLChild->SetValue("","colour", (long)m_prop.m_colorText);
   pXMLChild->SetValue("", "overlap", m_prop.m_bOverlap);
   pXMLChild->SetValue("", "textpos", m_prop.m_nTextPos);
   pXMLChild->SetValue("", "scalefont", m_prop.m_bScaleFont);
   pXMLChild->SetValue("", "font", m_prop.m_logfont);

   // Set the legend properties

   pXMLChild = pXMLObj->AddChild("legend");
   pXMLChild->SetValue("", "comment", m_prop.m_sComment);
   pXMLChild->SetValue("", "showlayername", m_prop.m_bShowLayerName);
   pXMLChild->SetValue("", "font", m_prop.m_logfontC);

   // Set the layer properties

   pXMLChild = pXMLObj->AddChild("layerstyle");
   
   // Separate colour and legend values

   if (m_prop.m_nSepColour != 0)
   {
      pXMLChild1 = pXMLChild->AddChild("separatecolours");
      pXMLChild1->SetValue("", "type", m_prop.m_nSepColour);
      m_prop.m_aColourFeature.AsXML(pXMLChild1); 
      
   }

   // Colour range

   if (m_prop.m_bRangeColour)
   {
      pXMLChild1 = pXMLChild->AddChild("colourranges");
      m_prop.m_aRangeColour.AsXML(pXMLChild1);
   }

   // Auto colour

   if (m_prop.m_bAutoColour || m_prop.m_bAutoSize)
   {
      pXMLChild1 = pXMLChild->AddChild("autostyle");
      pXMLChild1->SetValue("", "autocolour", m_prop.m_bAutoColour);
      pXMLChild1->SetValue("", "autosize", m_prop.m_bAutoSize);
      pXMLChild1->SetValue("", "min", m_prop.m_dAutoMin);
      pXMLChild1->SetValue("", "max", m_prop.m_dAutoMax);
   }
 
   // Save the query

   if (m_pQuery != NULL)
   {
      m_pQuery->AsXML(pXMLObj->AddChild("query"));
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// Initialise CMapLayer object from XML object
//

BOOL CMapLayer::XMLAs(CXMLObj *pXMLObj)
{
   BOOL bOK = TRUE;
   CXMLObj *pXMLChild = NULL, *pXMLChild1 = NULL;

   // Retrieve layer properties

   m_prop.m_sName = pXMLObj->GetString("","name");
   m_prop.m_bVisible = pXMLObj->GetInteger("","visible");
   
   // Get the style properties

   pXMLChild = pXMLObj->GetXMLObj("","style");
   if (pXMLChild == NULL || !m_prop.XMLAs(pXMLChild)) bOK = FALSE;

   m_prop.m_bBestFitSym = pXMLChild->GetInteger("", "bestfitsymbol");
   m_prop.m_bPolygon = pXMLChild->GetInteger("", "fillpolygon");

   // Get the text properties

   pXMLChild = pXMLObj->GetXMLObj("", "text", TRUE);
   m_prop.m_colorText = pXMLChild->GetInteger("","colour");
   m_prop.m_bOverlap = pXMLChild->GetInteger("", "overlap");
   m_prop.m_nTextPos = pXMLChild->GetInteger("", "textpos");
   m_prop.m_bScaleFont = pXMLChild->GetInteger("", "scalefont");
   m_prop.m_logfont = pXMLChild->GetLogfont("", "font");
   
   // Set the legend properties

   pXMLChild = pXMLObj->GetXMLObj("", "legend", TRUE);
   m_prop.m_sComment = pXMLChild->GetString("", "comment");
   m_prop.m_bShowLayerName = pXMLChild->GetInteger("", "showlayername");
   m_prop.m_logfontC = pXMLChild->GetLogfont("", "font");
   
   // Set the layer properties

   pXMLChild = pXMLObj->GetXMLObj("","layerstyle", TRUE);
   
   // Separate colour and legend values

   pXMLChild1 = pXMLChild->GetXMLObj("", "separatecolours");
   if (pXMLChild1 != NULL)
   {
      m_prop.m_nSepColour = pXMLChild1->GetInteger("", "type");
      if (m_prop.m_nSepColour != 0)
      {
         m_prop.m_aColourFeature.XMLAs(pXMLChild1); 
      }
   }

   // Colour range

   pXMLChild1 = pXMLChild->GetXMLObj("", "colourranges");
   if (pXMLChild1 != NULL)
   {
      m_prop.m_bRangeColour = TRUE;
      m_prop.m_aRangeColour.XMLAs(pXMLChild1);
   }

   // Auto colour

   pXMLChild1 = pXMLChild->GetXMLObj("", "autostyle");
   if (pXMLChild1 != NULL)
   {
      m_prop.m_bAutoColour = pXMLChild1->GetInteger("", "autocolour");
      m_prop.m_bAutoSize = pXMLChild1->GetInteger("", "autosize");
      m_prop.m_dAutoMin = pXMLChild1->GetDouble("", "min");
      m_prop.m_dAutoMax = pXMLChild1->GetDouble("", "max");
   }
 
   // Save the query

   ASSERT(m_pQuery == NULL);
   m_pQuery = new CQuery;
   if (m_pQuery != NULL) 
   {
      if (!m_pQuery->XMLAs(pXMLObj->GetXMLObj("","query", TRUE)))
      {
         bOK = FALSE;
      }
   }

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

int CMapLayer::Read(CMapLayerArray* pMapLayerArray, FILE* pFile)
{
   BOOL bOK = TRUE;   

   CString s = BDNextItem(pFile);
   
   if (s == "begin")
   {
      // Read the map layer attributes

      // Backwards compatability       
      CString s = BDNextStr(pFile,"title");
      if (s != "") pMapLayerArray->m_sTitle = s;      
      BDNext(pFile, &pMapLayerArray->m_logfontT, "logfontT");
      BDNext(pFile, &pMapLayerArray->m_logfontL, "logfontL");
      BOOL b = BDNextInt(pFile, "legend1Font", -1);
      if (b!= -1) pMapLayerArray->m_bLegend1Font = b;
      
      m_prop.m_sName = BDNextStr(pFile, "name");                  
      BDNextRGB(pFile, "color"); //Backwards compatibility
      m_prop.m_colorText = BDNextRGB(pFile, "textcolor");
      m_prop.m_nSymbol = BDNextInt(pFile);
      m_prop.m_dSymSize = BDNextDouble(pFile, FALSE);
      m_prop.m_crLine = BDNextInt(pFile, "colorline");
      m_prop.m_crFill = BDNextInt(pFile, "colorfill");
      m_prop.m_nPattern = BDNextInt(pFile, "pattern");
      m_prop.m_nHatch = BDNextInt(pFile, "hatch");      
      m_prop.m_nLineStyle = BDNextInt(pFile);
      m_prop.m_nLineWidth = BDNextInt(pFile, FALSE);
      m_prop.m_bOverlap = BDNextInt(pFile);      
      m_prop.m_nTextPos = BDNextInt(pFile, FALSE);

      // Read comment, restore line feeds
      m_prop.m_sComment = BDNextStr(pFile,"comment");            
      m_prop.m_bBestFitSym = BDNextInt(pFile,"fitsym");
      m_prop.m_bShowLayerName = BDNextInt(pFile,"showlayername");      
      m_prop.m_bVisible = BDNextInt(pFile,"visible", TRUE);
      m_prop.m_nSepColour = BDNextInt(pFile,"sepcolor");
      if (m_prop.m_nSepColour)
      {
         m_prop.m_aColourFeature.Read(pFile, m_prop.m_nSepColour);
      };
      m_prop.m_bRangeColour = BDNextInt(pFile,"rangecolor");
      if (m_prop.m_bRangeColour)
      {
         m_prop.m_aRangeColour.Read(pFile);
      }
      m_prop.m_bScaleFont = BDNextInt(pFile,"scalefont");
      m_prop.m_bAutoColour = BDNextInt(pFile, TRUE);
      m_prop.m_bAutoSize = BDNextInt(pFile, TRUE);
      m_prop.m_bPolygon = BDNextInt(pFile, TRUE);
      m_prop.m_dAutoMin = BDNextDouble(pFile);
      m_prop.m_dAutoMax = BDNextDouble(pFile);                 
      BDNext(pFile, &m_prop.m_logfont,"logfont");
      BDNext(pFile, &m_prop.m_logfontC,"logfontC");

      m_sFile = BDNextStr(pFile, "filename");
	  m_sColumn = BDNextStr(pFile, "column");

      s = BDNextItem(pFile);

      // Query

      if (s == "Query")
      {
         ASSERT(m_pQuery == NULL);
         m_pQuery = new CQuery;
         bOK = m_pQuery->Read(pFile);
      }
      
      // Read the map layer objects 

      else
      {      
         if (m_sFile == "")
         {
            m_lFType = BDNextInt(pFile);                  
            BDNext(pFile, &m_aAttr, m_lFType);      

            m_alFeatures.RemoveAll();
            BDNext(pFile, m_alFeatures);                  
         };
      };
      s = BDNextItem(pFile);


      if (s != "end") bOK = FALSE;
   } 
   else
   {
      return -1;
   }   

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

/*void CColourFeatureArray::Write(FILE* pFile, int nSepColour)
{
   fprintf(pFile,"numcolor=%li\n",GetSize());
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      fprintf(pFile,"colorfeature=%li,\"%s\"",GetAt(i).m_lFeatureId, GetAt(i).m_sFeature);
      if (nSepColour == CMapLayer::SepColourAttr || nSepColour == CMapLayer::LegendValues)
      {
         fprintf(pFile, ",\"%s\"", GetAt(i).m_sAttr);
      }
      fprintf(pFile, ",%li\n", GetAt(i).m_crFill); 
      fprintf(pFile, "mapstyle=%li", GetAt(i).m_crLine); 
      fprintf(pFile, ",%li", GetAt(i).m_nLineStyle); 
      fprintf(pFile, ",%li", GetAt(i).m_nLineWidth); 
      fprintf(pFile, ",%li", GetAt(i).m_nPattern); 
      fprintf(pFile, ",%li",GetAt(i).m_nHatch); 
      fprintf(pFile, ",%li",GetAt(i).m_nSymbol); 
      fprintf(pFile, ",%lf\n",GetAt(i).m_dSymSize); 
   }
}*/

/////////////////////////////////////////////////////////////////////////////

void CColourFeatureArray::AsXML(CXMLObj* pXMLObj)
{
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      CXMLObj *pXMLChild = pXMLObj->AddChild("separatecolour");
      pXMLChild->SetValue("", "featureid", GetAt(i).m_lFeatureId);
      pXMLChild->SetValue("", "name", GetAt(i).m_sFeature);
      if (GetAt(i).m_sAttr != "") pXMLChild->SetValue("", "attr", GetAt(i).m_sAttr);

      pXMLChild = pXMLChild->AddChild("style");
      GetAt(i).CMapStyle::AsXML(pXMLChild);
   }
}

/////////////////////////////////////////////////////////////////////////////

BOOL CColourFeatureArray::XMLAs(CXMLObj *pXMLObj)
{
   CXMLObj *pXMLChild = NULL;
   CColourFeature colourfeature;
   BOOL bOK = TRUE;

   BOOL bFound = pXMLObj->GetFirst("", "separatecolour", pXMLChild);
   while (bFound)
   {
      colourfeature = CColourFeature();
      colourfeature.m_lFeatureId = pXMLChild->GetInteger("", "featureid");
      colourfeature.m_sFeature = pXMLChild->GetString("", "name");
      colourfeature.m_sAttr = pXMLChild->GetString("", "attr");

      CXMLObj *pXMLStyle = pXMLChild->GetXMLObj("", "style");
      if (pXMLStyle != NULL)
      {
         colourfeature.CMapStyle::XMLAs(pXMLStyle);
      } else
      {
         bOK = TRUE;
      }

      Add(colourfeature);

      bFound = pXMLObj->GetNext("", "separatecolour", pXMLChild);
   }
   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CColourFeatureArray::Read(FILE* pFile, int nSepColour)
{
   BOOL bOK = TRUE;
   RemoveAll();

   int n = BDNextInt(pFile);
   int i = 0; for (i = 0; i < n; i++)
   {
      CColourFeature feature;
      feature.m_lFeatureId = BDNextInt(pFile,TRUE);
      feature.m_sFeature = BDNextStr(pFile,FALSE);
      if (nSepColour == CMapLayer::SepColourAttr || nSepColour == CMapLayer::LegendValues)
      {
         feature.m_sAttr = BDNextStr(pFile,FALSE);
      }
      feature.m_crFill = BDNextInt(pFile,FALSE); 

      // Read addional attributes if available - NRDB Pro 2.0

      int n = BDNextInt(pFile, "mapstyle", -1);
      if (n != -1)
      {
         feature.m_crLine = n;
         feature.m_nLineStyle = BDNextInt(pFile, FALSE);
         feature.m_nLineWidth = BDNextInt(pFile, FALSE);
         feature.m_nPattern = BDNextInt(pFile, FALSE);
         feature.m_nHatch = BDNextInt(pFile, FALSE);
         feature.m_nSymbol = BDNextInt(pFile, FALSE);
         feature.m_dSymSize = BDNextDouble(pFile, FALSE);
      }

      Add(feature);
   }
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

CColourRangeArray& CColourRangeArray::operator=(CColourRangeArray &rSrc)
{
   m_dRound = rSrc.m_dRound;
   m_sName = rSrc.m_sName;
   m_bApplyPercent = rSrc.m_bApplyPercent;
   Copy(rSrc);

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

CColourRangeArray::CColourRangeArray(CColourRangeArray& rSrc)
{
   *this = rSrc;
}

///////////////////////////////////////////////////////////////////////////////

/*void CColourRangeArray::Write(FILE* pFile)
{
   CString s;

   fprintf(pFile,"numcolor=%li\n",GetSize());
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      fprintf(pFile,"colorfeature=%lf,%lf\n",GetAt(i).m_dMin, GetAt(i).m_dMax);      
      
      s = ((CMapStyle&)GetAt(i)).AsString();
      fprintf(pFile, "mapstyle=%s\n", (LPCSTR)s);
   }
   fprintf(pFile, "round=%lf\n", m_dRound);
}*/

///////////////////////////////////////////////////////////////////////////////

BOOL CColourRangeArray::Read(FILE* pFile)
{
   BOOL bOK = TRUE;
   RemoveAll();

   int n = BDNextInt(pFile);
   int i = 0; for (i = 0; i < n; i++)
   {
      CColourRange rangecolor;
      rangecolor.m_dMin = BDNextDouble(pFile, TRUE);
      rangecolor.m_dMax = BDNextDouble(pFile, FALSE);      

      CString s = BDNextStr(pFile, FALSE);
      if (s == "mapstyle") BDNext(pFile, (CMapStyle&)rangecolor);
      else sscanf(s, "%d", &rangecolor.m_crFill);           
       
      Add(rangecolor);
   }
   m_dRound = BDNextDouble(pFile, "round", 1);
   

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the text description of the selected item, e.g. 0 <= n < 1
//

CString CColourRangeArray::GetDesc(int i)
{
   CString s;

   // Count number of decimal places

   std::strstream sValue;   

   if (m_dRound != 0)
   {
      int nDP = 0;
      CString sFormat;
      sFormat.Format("%lf", m_dRound);
      int k = sFormat.Find('.');
      int j = sFormat.Find('1',k+1);
      if (j > -1 && k > -1) nDP = j-k;
      
	  sValue.flags(std::ios::fixed);
      sValue.precision(nDP);
   };

   // Zero condition

   if (GetAt(i).m_dMin == GetAt(i).m_dMax)
   {
      sValue << GetAt(i).m_dMin << std::ends;
   } 

   // Next to zero

   else if (i > 0 && GetAt(i-1).m_dMin == GetAt(i-1).m_dMax)
   {    
      sValue << GetAt(i).m_dMin << " < n < " << GetAt(i).m_dMax << std::ends;
   }
   
   // Other conditions
   else
   {
      sValue << GetAt(i).m_dMin << " <= n < " << GetAt(i).m_dMax << std::ends;
      


   };

   s = sValue.str();
   sValue.rdbuf()->freeze(0);
   return s;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CMapLayer::Initialise(CQuery* pQuery)
{
   BOOL bOK = TRUE;
   m_pQuery = pQuery;
   m_lFType = m_pQuery->GetFType();

   // Create the map objects based on the query results

   if (m_prop.m_sName == "")
   {
      m_prop.m_sName = m_pQuery->GetQueryName();
   };

   CQueryResult queryresult;
   if (queryresult.Initialise(pQuery))
   {
      // Copy the attributes from the query result

      m_aAttr.RemoveAll();
      int i = 0; for (i = 0; i < queryresult.GetAttrArray().GetSize(); i++) 
      {
         m_aAttr.Add(queryresult.GetAttrArray().GetAt(i));      
      };

      // Retrieve map attributes

	  BDProgressText("Processing map data...");
	  BDProgressRange(0, queryresult.GetSize());

      for (i = 0; i < queryresult.GetSize(); i++)
      {
         CMapLayerObj* pMapObj = new CMapLayerObj;

         pMapObj->SetFeature(queryresult.GetAt(i)->m_lFeature);      
        
         if (AddAttribute(pMapObj, *queryresult[i]))
         {          
            Add(pMapObj);
         } else
         {
            delete pMapObj;
         }

		 BDProgressPos(i);
      }  
   } else
   {
      bOK = FALSE;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void CMapLayerArray::OnLoad(LPCSTR psFile)
{ 
   CString sPathName;      
 
   // If no file specified then display file open dialog

   if (psFile == NULL)
   {
      CFileDialog dlg(TRUE, "nrm", NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
                      BDString(IDS_NRDBFILE) + "|*.nrm||");
      if (dlg.DoModal() == IDOK)
      {
         sPathName = dlg.GetPathName();
      

      // If existing layers then ask whether to replace

         if (GetSize() != 0)
         {
            if (AfxMessageBox(BDString(IDS_REPLACELAYERS), MB_YESNO+MB_DEFBUTTON2) == IDYES)
            {
               m_sTitle = "";
               RemoveAll();
               
            }
         }
      };
   } else
   {
      sPathName = psFile;
   }
   
   // Redraw the screen

   AfxGetMainWnd()->UpdateWindow();      
   AfxGetMainWnd()->RedrawWindow();

   // Import the layers

   if (sPathName != "")
   {
      CDlgProgress dlgProgress;

      CMapLayerArray aMapLayers;      

      TRY
      {
	      if (aMapLayers.Read(sPathName))
         {
            int nSets = aMapLayers.GetSize();
            int nSet = 0;
            int i = 0; for (i = 0; i < aMapLayers.GetSize(); i++)
            {
               Add(aMapLayers[i]);
               aMapLayers.RemoveAt(i--); // Don't delete pointers            
            };            
         
            // Copy maplayers properties
            *this = aMapLayers;
         } 
         else
         {         
            AfxThrowFileException(0,0,NULL);            
         }      
      }
      CATCH(CFileException, ex)
      {
         if (m_sError == "") m_sError = "\r\n" + m_sError;
         AfxMessageBox(BDString(IDS_ERROROPEN) + ": " + sPathName + m_sError);		                 
         m_sError = "";
      }
      END_CATCH
   };
}

///////////////////////////////////////////////////////////////////////////////

CColourFeature::CColourFeature()
{
   m_lFeatureId = 0;      
}

