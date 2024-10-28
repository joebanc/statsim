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
#include <io.h>

#include "nrdb.h"
#include "PageMapLayerProp.h"
#include "viewmap.h"
#include "docmap.h"
#include "definitions.h"
#include "sheetmapprop.h"
#include "dlgsavescheme.h"
#include "xmlfile.h"
#include "dlgcolourrange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define SCHEMEFILE "schemes.xml"

/////////////////////////////////////////////////////////////////////////////

inline void swap(double& d1, double& d2)
{
   double dTemp = d1;
   d1 = d2;
   d2 = dTemp;
}

#define  HLSMAX   240   /* H,L, and S vary over 0-HLSMAX */
#define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */
                        /* HLSMAX BEST IF DIVISIBLE BY 6 */
                        /* RGBMAX, HLSMAX must each fit in a byte. */
#define UNDEFINED (HLSMAX*2/3)


DWORD HLStoRGB(WORD hue, WORD lum, WORD sat);



/////////////////////////////////////////////////////////////////////////////
// CPageMapLayerProp property page

IMPLEMENT_DYNCREATE(CPageMapLayerProp, CPropertyPage)

CPageMapLayerProp::CPageMapLayerProp()
{
}

CPageMapLayerProp::CPageMapLayerProp(CMapLayer* pMapLayer, CMapProperties* pMapProperty) : 
   CPropertyPage(CPageMapLayerProp::IDD)
{
   m_pMapLayer = pMapLayer;
   m_pMapProperty = pMapProperty;
   m_bCoord = FALSE;
   m_bMapLines = FALSE;

	//{{AFX_DATA_INIT(CPageMapLayerProp)
	//}}AFX_DATA_INIT
}

CPageMapLayerProp::~CPageMapLayerProp()
{
}

void CPageMapLayerProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMapLayerProp)
	DDX_Control(pDX, IDC_SCHEMEMENU, m_pbSchemeMenu);
	DDX_Control(pDX, IDC_COLOURSCHEME, m_cbColourScheme);
	DDX_Control(pDX, IDC_AUTOSTYLE, m_cbAutoStyle);
	DDX_Control(pDX, IDC_FEATURENAME, m_eFeatureName);	
	DDX_Control(pDX, IDC_LAYERS, m_lbLayers);   
	DDX_Control(pDX, IDC_MIN, m_eMin);
	DDX_Control(pDX, IDC_MAX, m_eMax);
   //DDX_Control(pDX, IDC_COLOR, m_pbColour);   
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CPageMapLayerProp, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMapLayerProp)   
	ON_LBN_SELCHANGE(IDC_LAYERS, OnSelchangeLayers)
	ON_EN_CHANGE(IDC_FEATURENAME, OnChangeFeaturename)	
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN, OnDeltaposUpdown)
	ON_CBN_SELCHANGE(IDC_AUTOSTYLE, OnSelchangeAutostyle)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_CBN_SELCHANGE(IDC_COLOURSCHEME, OnSelchangeColourscheme)		
	ON_COMMAND(ID_SCHEME_SAVE, OnSchemeSave)
	ON_COMMAND(ID_SCHEME_DELETE, OnSchemeDelete)	
   ON_BN_CLICKED(IDC_SCHEMEMENU, OnSchememenu)
	//}}AFX_MSG_MAP
   ON_COMMAND(ID_ADVANCED, OnAdvanced)  

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMapLayerProp message handlers

BOOL CPageMapLayerProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Scan the map objects to determine which properties to display
   
   int i = 0; for (i = 0; i < m_pMapLayer->GetSize(); i++)
   {
      CMapLayerObj* pMapLayerObj = (CMapLayerObj*)m_pMapLayer->GetAt(i);
      if (pMapLayerObj->GetDataType() == BDCOORD) m_bCoord = TRUE;
      if (pMapLayerObj->GetDataType() == BDMAPLINES) m_bMapLines = TRUE;
   }


   // Fill list of available styles

   int index = m_cbAutoStyle.AddString(BDString(IDS_NONE));
   m_cbAutoStyle.SetItemData(index,0);
   m_cbAutoStyle.SetCurSel(index);
   if (m_bCoord)
   {
      index = m_cbAutoStyle.AddString(BDString(IDS_AUTOSIZE));
      m_cbAutoStyle.SetItemData(index,autosize);
   };
   index = m_cbAutoStyle.AddString(BDString(IDS_AUTOCOLOR) + " (" + BDString(IDS_LIGHTTODARK) +")");
   m_cbAutoStyle.SetItemData(index,autocolor); 
   index = m_cbAutoStyle.AddString(BDString(IDS_SEPARATECOLOR));
   m_cbAutoStyle.SetItemData(index,sepcolor);
   index = m_cbAutoStyle.AddString(BDString(IDS_COLORRANGE));
   m_cbAutoStyle.SetItemData(index,rangecolor);
   index = m_cbAutoStyle.AddString(BDString(IDS_LEGENDVALUES));
   m_cbAutoStyle.SetItemData(index,legendvalues);   
        
   // Initialise controls

   //m_pbColour.SetColour(m_pMapLayer->GetColour());

   // Set combo box for style
      
   for (i = 0; i < m_cbAutoStyle.GetCount(); i++)
   {
      if ((m_cbAutoStyle.GetItemData(i) == autocolor && m_pMapLayer->GetAutoColour() || 
          m_cbAutoStyle.GetItemData(i) == autosize && m_pMapLayer->GetAutoSize()) && 
          m_pMapLayer->GetAutoMin() < m_pMapLayer->GetAutoMax())
      {
         m_cbAutoStyle.SetCurSel(i);                  
      }             
      if (m_cbAutoStyle.GetItemData(i) == sepcolor && (m_pMapLayer->GetSepColour() == CMapLayer::SepColourAttr || 
          m_pMapLayer->GetSepColour() == CMapLayer::SepColourFeature))
      {
         m_cbAutoStyle.SetCurSel(i);         
      }
      if (m_cbAutoStyle.GetItemData(i) == legendvalues && m_pMapLayer->GetSepColour() == CMapLayer::LegendValues)
      {
         m_cbAutoStyle.SetCurSel(i);            
      }
      if (m_cbAutoStyle.GetItemData(i) == rangecolor && m_pMapLayer->GetRangeColour())
      {
         m_cbAutoStyle.SetCurSel(i);
      }
   }

   // Initialise auto color / separate color

   OnAutocolor();   
   double dMin = m_pMapLayer->GetAutoMin();
   double dMax = m_pMapLayer->GetAutoMax();
   if (dMin > dMax) swap(dMin, dMax);
   m_eMin.SetValue(dMin);
   m_eMax.SetValue(dMax);      
   m_nSepColour = m_pMapLayer->GetSepColour();
   SepColour(TRUE);   
   RangeColour(TRUE);

   // Indicate whether the separate layers list displays symbols or map lines

   m_lbLayers.SetMode(m_bCoord ? CViewMap::points : (m_pMapProperty->m_bPolygon ? CViewMap::polygon : CViewMap::polyline));

// Disable for legends

   if (!m_bCoord && !m_bMapLines)
   {
      m_cbAutoStyle.EnableWindow(FALSE);
   }

// Initialise colour schemes

   InitSchemes();   

   // Initialise bitmap for scheme load/save

   m_bitmapPopup.LoadBitmap(IDB_POPUPMENU);
   m_pbSchemeMenu.SetBitmap(m_bitmapPopup);
      
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnSelchangeAutostyle() 
{
   int index = m_cbAutoStyle.GetCurSel();
   if (index != CB_ERR)
   {      
       RestoreSchemes();
       OnAutocolor();
       SepColour();
       RangeColour();       
   }
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnAutocolor() 
{
   int index = m_cbAutoStyle.GetCurSel();
   ASSERT(index != CB_ERR);
   DWORD dw = m_cbAutoStyle.GetItemData(index);
   BOOL bAutoColour = dw == autocolor || dw == autosize || dw == ranking;

   InitControls();
      
   // Determine default values for maximum and minimum
  
   if (m_pMapLayer->GetSize() > 0)
   {     
      double dMin, dMax;

      if (GetMinMax(m_pMapLayer, dMin, dMax))
      {                      
         m_eMin.SetValue(dMin);
         m_eMax.SetValue(dMax);      
      };
   }   
};

/////////////////////////////////////////////////////////////////////////////

BOOL CPageMapLayerProp::GetMinMax(CMapLayer* pMapLayer, double& dMin, double& dMax)
{
   dMin = DBL_MAX;
   dMax = -DBL_MAX;

   int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
   {
      CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(j);
      if (pMapLayerObj->GetValue() != NULL_READING && 
          pMapLayerObj->GetValue() != NULL_READING_OLD)
      {
         dMin = min(dMin, pMapLayerObj->GetValue());
         dMax = max(dMax, pMapLayerObj->GetValue());         
      };
   }   

   if (dMin == DBL_MAX || IsNullDouble(dMax)) return FALSE;
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Each feature, within a map layer is to have a separate color


void CPageMapLayerProp::SepColour(BOOL bInit)  
{  
   // Enable the corresponding list box
   // Prevent auto color and auto size

   int index = m_cbAutoStyle.GetCurSel();
   ASSERT(index != CB_ERR);
   DWORD dw = m_cbAutoStyle.GetItemData(index);
   BOOL bCheck = dw == sepcolor || dw == legendvalues;

   InitControls();
   
   // Determine unique colours

   if (bCheck)
   {
      // If list is empty then restore

      CColourFeatureArray& aColourFeature = m_pMapProperty->m_aColourFeature;

      // Set the default colour scheme

      if (m_cbColourScheme.GetCurSel() == CB_ERR) m_cbColourScheme.SetCurSel(0);
      
      if (!bInit)
      {      
         BeginWaitCursor();
         aColourFeature.RemoveAll();

         // If legend values then just retrieve values

         if (dw == legendvalues)
         {
            m_nSepColour = CMapLayer::LegendValues;            

            int i = 0; for (i = 0; i < m_pMapLayer->GetSize(); i++)
            {         
               CMapLayerObj* pMapObj = m_pMapLayer->GetAt(i);
               CColourFeature feature;
               feature.m_lFeatureId = pMapObj->GetFeature();
               
               if (!IsNullDouble(pMapObj->GetValue()))
               {
				   std::strstream ss;                     
                  ss.precision(10);
                  ss << pMapObj->GetValue() << std::ends;
                  feature.m_sAttr = ss.str();
                  ss.rdbuf()->freeze(0);         
               };               
               aColourFeature.Add(feature);
            };
         }
                  
         // Determine the number of unique features, if equal to the
         // number of items then sort by attribute
      
         else 
         {
            m_nSepColour = CMapLayer::SepColourFeature;
            int i = 0; for (i = 0; i < m_pMapLayer->GetSize(); i++)
            {
               CMapLayerObj* pMapObj = m_pMapLayer->GetAt(i);

               if (!IsNullDouble(pMapObj->GetValue()) && 
				   pMapObj->GetFeature() != 0)
               {               
                  break;
               }
               else if (CString(pMapObj->GetText()) != "")
               {
                  m_nSepColour = CMapLayer::SepColourAttr;
                  break;
               }            
            }
         
            // Search for unique features / attribute

            for (i = 0; i < m_pMapLayer->GetSize(); i++)
            {         
               CMapLayerObj* pMapObj = m_pMapLayer->GetAt(i);

               // Search for existing

               int j = 0; for (j = 0; j < aColourFeature.GetSize(); j++)
               {
                  if (m_nSepColour == CMapLayer::SepColourFeature && 
                      aColourFeature[j].m_lFeatureId == pMapObj->GetFeature() && 
					  pMapObj->GetFeature() != 0)
                  {
                     break;
                  }
                  else if (m_nSepColour != CMapLayer::SepColourFeature && 
                           aColourFeature[j].m_sAttr == pMapObj->GetText())
                  {
                     break;
                  }
               }

               // Not found then add

               if (j == aColourFeature.GetSize())
               {
                  CColourFeature feature;
                  if (m_nSepColour == CMapLayer::SepColourFeature) 
                  {
                     feature.m_lFeatureId = pMapObj->GetFeature();
                  }
                  else 
                  {
                     feature.m_sAttr = pMapObj->GetText();
                     feature.m_sFeature = pMapObj->GetText();
                  };
                  aColourFeature.Add(feature);
               }
            }            
         };

         // Now determine the names of features

         if (m_nSepColour == CMapLayer::SepColourFeature || 
             m_nSepColour == CMapLayer::LegendValues)
         { 
            CFeature feature;
            feature.m_lFeatureTypeId = m_pMapLayer->GetFType();
            BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
            while (bFound)
            {
               int j = 0; for (j = 0; j < aColourFeature.GetSize(); j++)
               {
                  if (aColourFeature[j].m_lFeatureId == feature.m_lId)
                  {
                     aColourFeature[j].m_sFeature = feature.m_sName;
                  }
               }
               bFound = BDGetNext(BDHandle());
            }
            BDEnd(BDHandle());
         };

         // Now determine colors for the features
         
         int j = 0; for (j = 0; j < aColourFeature.GetSize(); j++)
         {            
            CMapStyle mapstyle = GetStyle(j);
            (CMapStyle&)aColourFeature[j] = mapstyle;            
         }
         
         EndWaitCursor();
      }

      // Now add the features to the list

      m_lbLayers.ResetContent();
      int j = 0; for (j = 0; j < aColourFeature.GetSize(); j++)
      {         
         m_lbLayers.AddString(aColourFeature[j].m_sFeature + "\n" + aColourFeature[j].m_sAttr,                              
                              aColourFeature[j].m_lFeatureId, 
                              (CMapStyle&)aColourFeature[j]);
      };      
      m_lbLayers.SetCurSel(0);
      OnSelchangeLayers();
   }
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::RangeColour(BOOL bInit)
{
   // Determine whether range color is selected

   int index = m_cbAutoStyle.GetCurSel();
   ASSERT(index != CB_ERR);
   DWORD dw = m_cbAutoStyle.GetItemData(index);
   BOOL bCheck = dw == rangecolor;

   // Enable controls

   InitControls();

   // Initialise

   if (bCheck)
   {
      CColourRangeArray& aRangeColour = m_pMapProperty->m_aRangeColour;
      
      // Restore colour ranges from selection

      index = m_cbColourScheme.GetCurSel();
      if (index != CB_ERR && !bInit)
      {
         aRangeColour = m_aRangeSchemes[index];

         // Apply ranges as a percentage of the maximum and minimum values

         if (aRangeColour.m_bApplyPercent)
         {
            double dMin, dMax;
            if (GetMinMax(m_pMapLayer, dMin, dMax))
            {
               // If the first range is from zero then set the minimum value to zero (unless negative)

               if (aRangeColour.GetSize()>0 && fabs(aRangeColour[0].m_dMin) < 0.001 && dMin > 0) dMin = 0;

               // Apply the percentages to each colour range based on max and min values
               int i = 0; for (i = 0; i < aRangeColour.GetSize(); i++)
               {
                  aRangeColour[i].m_dMin = dMin + aRangeColour[i].m_dMin/100 * (dMax-dMin);
                  aRangeColour[i].m_dMax = dMin + aRangeColour[i].m_dMax/100 * (dMax-dMin);
               }
            }
         }
      }
      else if (aRangeColour.GetSize() == 0 || !bInit)
      {
         BeginWaitCursor();         

         CDlgColourRange dlg(m_pMapLayer, m_pMapProperty, TRUE);
         dlg.DoModal();         
      }

      

      // Now add the layers to the list

      m_lbLayers.ResetContent();
      int j = 0; for (j = 0; j < aRangeColour.GetSize(); j++)
      {                  
         CString s = aRangeColour.GetDesc(j);
         index = m_lbLayers.AddString(s + "\n" , 0, (CMapStyle&)aRangeColour[j]);
      }; 
      m_eMin.SetWindowText("");
      m_eMax.SetWindowText("");
      m_lbLayers.SetCurSel(0);
      OnSelchangeLayers();

      EndWaitCursor();
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// Retrieve selected colors
//

void CPageMapLayerProp::UpdateRangeColour()
{
   CMapStyle mapstyle;
   // Retrieve colors

   CColourRangeArray& aRangeColour = m_pMapProperty->m_aRangeColour;
   ASSERT(aRangeColour.GetSize() == m_lbLayers.GetCount());
   int i = 0; for (i = 0; i < m_lbLayers.GetCount(); i++)
   {
      mapstyle = m_lbLayers.GetStyle(i);

      (CMapStyle&)aRangeColour[i] = mapstyle;
      
      
   };
}

/////////////////////////////////////////////////////////////////////////////
//
// Add a new value to a range of values
//

void CPageMapLayerProp::OnAdd() 
{
   double dMin, dMax;
   COLORREF cr = -1;

   // Store existing selected colors

   UpdateRangeColour();

   // Retrieve the values from the controls

   if (m_eMin.GetValue(dMin) && m_eMax.GetValue(dMax,dMin))   
   {
      CColourRangeArray& aRangeColour = m_pMapProperty->m_aRangeColour;

     // Remove items included in new range

      int i = 0; for (i = 0; i < aRangeColour.GetSize(); i++)
      {
         if (aRangeColour[i].m_dMin >= dMin && 
             aRangeColour[i].m_dMax <= dMax)
         {
            cr = aRangeColour[i].m_crFill;
            aRangeColour.RemoveAt(i);
            i--;
         }
      }

      // Determine where to place the value in the list

      for (i = 0; i < aRangeColour.GetSize(); i++)
      {         
         // Insert the new value in the correct place

         if (aRangeColour[i].m_dMin >= dMin)
         {
            if (cr == -1)
            {
               if (i > 0) cr = aRangeColour[i-1].m_crFill;            
               else cr = aRangeColour[i].m_crFill;
            };
            break;
         }

         // Adjust lesser value to fit
         
         if (aRangeColour[i].m_dMax > dMin)
         {
            cr = aRangeColour[i].m_crFill;
            aRangeColour[i].m_dMax = dMin;
         }
      }

      // Insert value

      if (i < aRangeColour.GetSize()) aRangeColour[i].m_dMin = dMax;      
            
      CColourRange rangecolor;            
      rangecolor.m_dMin = dMin;
      rangecolor.m_dMax = dMax;
      rangecolor.m_crFill = cr;
      aRangeColour.InsertAt(i, rangecolor);

     // Update the list

      RangeColour(TRUE);      
   }	
}

/////////////////////////////////////////////////////////////////////////////
//
// Remove the current selection from the list
//

void CPageMapLayerProp::OnDelete() 
{  
   CColourRangeArray& aRangeColour = m_pMapProperty->m_aRangeColour;
   ASSERT(aRangeColour.GetSize() == m_lbLayers.GetCount());
      
   int i = m_lbLayers.GetCurSel();   
   if (i != LB_ERR)
   {
      // Retrieve colours

      UpdateRangeColour();

      // Adjust values either side to fill gap   

      if (i > 0) aRangeColour[i-1].m_dMax = aRangeColour[i].m_dMax;
      if (i+1 < aRangeColour.GetSize()) aRangeColour[i+1].m_dMin = aRangeColour[i].m_dMin;
      
      // Remove value
   
      aRangeColour.RemoveAt(i);

      // Redraw

      RangeColour(TRUE);
   };
	   
}


/////////////////////////////////////////////////////////////////////////////
//
// Enable/Disbales appropriate controls according to Auto Style property
//


void CPageMapLayerProp::InitControls()
{
   int index = m_cbAutoStyle.GetCurSel();
   ASSERT(index != CB_ERR);
   DWORD dw = m_cbAutoStyle.GetItemData(index);   

   m_lbLayers.EnableWindow(dw == sepcolor || dw == rangecolor || dw == legendvalues); 
   m_lbLayers.ShowWindow(dw == sepcolor || dw == rangecolor || dw == legendvalues); 
   m_eFeatureName.EnableWindow(dw == sepcolor || dw == legendvalues);
   m_eFeatureName.ShowWindow(dw == sepcolor || dw == legendvalues);   
   m_cbColourScheme.ShowWindow(dw == sepcolor || dw == legendvalues || dw == rangecolor);
   GetDlgItem(IDC_SCHEMEMENU)->ShowWindow(dw == sepcolor || dw == legendvalues || dw == rangecolor);
   GetDlgItem(IDS_COLOURSCHEME)->ShowWindow(dw == sepcolor  || dw == legendvalues || dw == rangecolor);
   GetDlgItem(IDC_UPDOWN)->ShowWindow(dw == sepcolor || dw == legendvalues);   
   GetDlgItem(IDS_MIN)->ShowWindow(dw == autocolor || dw == autosize || dw == rangecolor || dw == ranking);
   GetDlgItem(IDS_MAX)->ShowWindow(dw == autocolor || dw == autosize || dw == rangecolor || dw == ranking);
   m_eMin.EnableWindow(dw == autocolor || dw == autosize || dw == rangecolor || dw == ranking);
   m_eMax.EnableWindow(dw == autocolor || dw == autosize || dw == rangecolor || dw == ranking);
   m_eMin.ShowWindow(dw == autocolor || dw == autosize || dw == rangecolor || dw == ranking);
   m_eMax.ShowWindow(dw == autocolor || dw == autosize || dw == rangecolor || dw == ranking);
   GetDlgItem(IDC_ADD)->ShowWindow(dw == rangecolor);
   GetDlgItem(IDC_DELETE)->ShowWindow(dw == rangecolor);   
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult) 
{  
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   
	int i = m_lbLayers.GetCurSel();

   m_lbLayers.UpDown(pNMUpDown->iDelta);
	
	*pResult = 0;

}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnSelchangeLayers() 
{
   int index = m_cbAutoStyle.GetCurSel();
   ASSERT(index != CB_ERR);
   DWORD dw = m_cbAutoStyle.GetItemData(index);
   
   if (dw == sepcolor)
   {
      int index = m_lbLayers.GetCurSel();
      if (index != LB_ERR)
      {      
         CString s = m_lbLayers.GetText(index);
         m_eFeatureName.SetWindowText(s.Left(s.Find('\n')));
      }	
   }
   else if (dw == legendvalues)
   {
      int index = m_lbLayers.GetCurSel();
      if (index != LB_ERR)
      {      
         CString s = m_lbLayers.GetText(index);         
         m_eFeatureName.SetWindowText(s.Mid(s.Find('\n')+1));
      }	
   }
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnChangeFeaturename() 
{
   int index = m_cbAutoStyle.GetCurSel();
   ASSERT(index != CB_ERR);
   DWORD dw = m_cbAutoStyle.GetItemData(index);

   // Separate Colour

   if (dw == sepcolor)
   {
	   CString s;
      m_eFeatureName.GetWindowText(s);
   
      int index = m_lbLayers.GetCurSel();
      if (index != LB_ERR)
      {
         CString s2 = m_lbLayers.GetText(index);
         s2 = s2.Mid(s2.Find('\n')+1);

         m_lbLayers.SetText(index, s + '\n' + s2);      
      }	
   }

   // Legend Values

   else if (dw == legendvalues)
   {
      CString s;
      m_eFeatureName.GetWindowText(s);
   
      int index = m_lbLayers.GetCurSel();
      if (index != LB_ERR)
      {
         CString s2 = m_lbLayers.GetText(index);
         s2 = s2.Left(s2.Find('\n'));

         m_lbLayers.SetText(index, s2 + '\n' + s);      
      }	
   }
   
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageMapLayerProp::OnSetActive() 
{
   // Set this as the default page

   CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
   ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CSheetMapProp)));
   pSheet->m_nDefaultPage = CSheetMapProp::layer;
	
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
//
// Save changes
//

BOOL CPageMapLayerProp::OnKillActive() 
{
   double d1 = NULL_DOUBLE, d2 = NULL_DOUBLE;
   CString str;
   CMapStyle mapstyle;

   int index = m_cbAutoStyle.GetCurSel();
   ASSERT(index != CB_ERR);
   DWORD dw = m_cbAutoStyle.GetItemData(index);
  
   if ((dw != autocolor && dw != autosize && dw != ranking) || 
       (m_eMin.GetValue(d1) && m_eMax.GetValue(d2, d1)))
   {        
      if (dw == ranking) 
      {
         swap(d1, d2);
         dw = autocolor;
      };
      
      // Auto Colour/Size properties

      m_pMapProperty->m_bAutoColour = dw == autocolor;
      m_pMapProperty->m_bAutoSize = dw == autosize;
      
      if (dw == autocolor || dw == autosize)
      {
         m_pMapProperty->m_dAutoMin = d1;
         m_pMapProperty->m_dAutoMax = d2;
      }

      // Separate color, Update list
      
      m_pMapProperty->m_nSepColour = 0;      
      if (dw == sepcolor || dw == legendvalues)
      {
         m_pMapProperty->m_nSepColour = m_nSepColour;       
         
         CColourFeatureArray& aColourFeature = m_pMapProperty->m_aColourFeature;
         aColourFeature.RemoveAll();

         int i = 0; for (i = 0; i < m_lbLayers.GetCount(); i++)
         {
            mapstyle = m_lbLayers.GetStyle(i);

            CColourFeature feature;
            feature.m_lFeatureId = m_lbLayers.GetItemData(i);
            (CMapStyle&)feature = mapstyle;            
            CString s = m_lbLayers.GetText(i);
            feature.m_sFeature = s.Left(s.Find('\n'));
            feature.m_sAttr = s.Mid(s.Find('\n')+1);
            aColourFeature.Add(feature);            
         }
      }

      // Range Colour, Update list

      m_pMapProperty->m_bRangeColour = dw == rangecolor;      
      if (dw == rangecolor)
      {
         UpdateRangeColour();
      }
      
	   return CPropertyPage::OnKillActive();
   };
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnOK() 
{
   m_pMapLayer->GetMapProp() = *m_pMapProperty;
	
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

CMapStyle CPageMapLayerProp::GetStyle(int nValue, int index)
{
   // Retrieve the colour according to the currently selected colour scheme

   if (index == -1) index = m_cbColourScheme.GetCurSel();
   if (index == -1 || m_aStyleSchemes[index].m_aStyle.GetSize() == 0) return CMapStyle();

   return m_aStyleSchemes[index].m_aStyle[nValue % m_aStyleSchemes[index].m_aStyle.GetSize()];
}

///////////////////////////////////////////////////////////////////////////////

CColourRange CPageMapLayerProp::GetColourRange(int nValue, int index)
{
   // Retrieve the colour and max/minaccording to the currently selected colour scheme

   if (index == -1) index = m_cbColourScheme.GetCurSel();
   if (index == -1 || m_aRangeSchemes[index].GetSize() == 0) return CColourRange();

   return m_aRangeSchemes[index][nValue % m_aRangeSchemes[index].GetSize()];

}

///////////////////////////////////////////////////////////////////////////////
//
// Change the colours to the new colour scheme
//
 
void CPageMapLayerProp::OnSelchangeColourscheme() 
{
   SepColour(FALSE);
   RangeColour(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
//
// Initialise the colour scheme
//

void CPageMapLayerProp::InitScheme(CStyleScheme& scheme, COLORREF crMin, COLORREF crMax)
{
   scheme.m_aStyle.RemoveAll();

   for (int lum = 220; lum >= 0; lum -= 60)
   {
      for (int sat = 240; sat >= 0; sat -= 40)      
      {
         for (int hue = 0; hue <= 240; hue += 15)         
         {
             COLORREF cr = HLStoRGB(hue, lum, sat);

            WORD r = GetRValue(cr);
            WORD g = GetGValue(cr);
            WORD b = GetBValue(cr);
             
            if (GetRValue(crMin) <= r && r <= GetRValue(crMax) && 
                GetGValue(crMin) <= g && g <= GetGValue(crMax) && 
                GetBValue(crMin) <= b && b <= GetBValue(crMax))
            {
                CMapStyle mapstyle;

                if (m_pMapProperty->m_bPolygon)
                {
                   mapstyle.m_crFill = RGB(r,g,b);
                } else
                {
                   mapstyle.m_crLine = RGB(r,g,b);
                }
                scheme.m_aStyle.Add(mapstyle);     
            }
  
         }
      }
   }
   
}


///////////////////////////////////////////////////////////////////////////////
//
// Source: Microsoft knowledge base
//

WORD HueToRGB(WORD n1, WORD n2, WORD hue)  
{ 

   /* range check: note values passed add/subtract thirds of range */
   if (hue < 0)
      hue += HLSMAX;

   if (hue > HLSMAX)
      hue -= HLSMAX;

   /* return r,g, or b value from this tridrant */
   if (hue < (HLSMAX/6))
      return ( n1 + (((n2-n1)*hue+(HLSMAX/12))/(HLSMAX/6)) );
   if (hue < (HLSMAX/2))
      return ( n2 );
   if (hue < ((HLSMAX*2)/3))
      return ( n1 + (((n2-n1)*(((HLSMAX*2)/3)-hue)+(HLSMAX/12))/(HLSMAX/6))

); 
   else
      return ( n1 );

} 

///////////////////////////////////////////////////////////////////////////////

DWORD HLStoRGB(WORD hue, WORD lum, WORD sat)
{ 

   WORD R,G,B;                /* RGB component values */
   WORD  Magic1,Magic2;       /* calculated magic numbers (really!) */

   if (sat == 0) {            /* achromatic case */
      R=G=B=(lum*RGBMAX)/HLSMAX;
      if (hue != UNDEFINED) {
         /* ERROR */
      }
   }
   else  {                    /* chromatic case */
      /* set up magic numbers */
      if (lum <= (HLSMAX/2))
         Magic2 = (lum*(HLSMAX + sat) + (HLSMAX/2))/HLSMAX;
      else
         Magic2 = lum + sat - ((lum*sat) + (HLSMAX/2))/HLSMAX;
      Magic1 = 2*lum-Magic2;

      /* get RGB, change units from HLSMAX to RGBMAX */
      R = (HueToRGB(Magic1,Magic2,hue+(HLSMAX/3))*RGBMAX +

(HLSMAX/2))/HLSMAX; 
      G = (HueToRGB(Magic1,Magic2,hue)*RGBMAX + (HLSMAX/2)) / HLSMAX;
      B = (HueToRGB(Magic1,Magic2,hue-(HLSMAX/3))*RGBMAX +

(HLSMAX/2))/HLSMAX; 
   }

   return(RGB(R,G,B));

} 

///////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::InitSchemes()
{
   CStyleScheme scheme;
   CMapStyle mapstyle;
   CString s, sStyle;

   BeginWaitCursor();

// Save existing values before re-initialising schemes

   OnKillActive();

// Remove existing schemes from list

   m_aStyleSchemes.RemoveAll();

// Load the custom schemes first

   CXMLFile xmlfile;
   if (!xmlfile.Read(BDGetAppPath() + SCHEMEFILE) && 
       access(BDGetAppPath() + SCHEMEFILE, 00) == 0)
   {
      AfxMessageBox("Error reading: " + BDGetAppPath() + SCHEMEFILE);
   }

// Convert xml object to scheme

   m_aStyleSchemes.XMLAs(&xmlfile);   
   m_aRangeSchemes.XMLAs(&xmlfile);

// Create default schemes

   scheme.m_bSystem = TRUE;
   scheme.m_sName = BDString(IDS_NATURAL);
   InitScheme(scheme, RGB(152,152,0), RGB(255,255,128));
   m_aStyleSchemes.Add(scheme);
   scheme.m_sName = BDString(IDS_BLUES);
   InitScheme(scheme, RGB(0,0,128), RGB(96,128,255));
   m_aStyleSchemes.Add(scheme);
   scheme.m_sName = BDString(IDS_GREENS);
   InitScheme(scheme, RGB(0,128,0), RGB(96,255,152));
   m_aStyleSchemes.Add(scheme);
   scheme.m_sName = BDString(IDS_REDS);
   InitScheme(scheme, RGB(196,0,0), RGB(255,96,96));
   m_aStyleSchemes.Add(scheme);
   scheme.m_sName = BDString(IDS_AQUATIC);
   InitScheme(scheme, RGB(0,96,96), RGB(48,255,255));
   m_aStyleSchemes.Add(scheme);
   scheme.m_sName = BDString(IDS_PASTELS);;
   InitScheme(scheme, RGB(152,152,152), RGB(255,255,255));
   m_aStyleSchemes.Add(scheme);

   RestoreSchemes();

   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////
//
// When the selection changes for the auto-style, initiatise list of saved
// schemes
//

void CPageMapLayerProp::RestoreSchemes()
{

   m_cbColourScheme.ResetContent();

   // Remember previous colour scheme

   DWORD dwStyle = m_cbAutoStyle.GetItemData(m_cbAutoStyle.GetCurSel());
   if (dwStyle == sepcolor || dwStyle == legendvalues)
   {
      int i = 0; for (i = 0; i < m_aStyleSchemes.GetSize(); i++)
      {      
         m_cbColourScheme.AddString(m_aStyleSchemes[i].m_sName);
      };

      for (i = 0; i < m_aStyleSchemes.GetSize(); i++)
      {
         CColourFeatureArray& aColourFeature = m_pMapProperty->m_aColourFeature;
         int j = 0; for (j = 0; j < aColourFeature.GetSize(); j++)
         {
            if (!(aColourFeature[j] == GetStyle(j, i))) break;
         }
         if (j == aColourFeature.GetSize())
         {
            m_cbColourScheme.SetCurSel(i);
            break;
         }
      };         
   }      

   //  Range colour scheme

   else if (dwStyle == rangecolor)
   {
      CColourRangeArray& aRangeColour = m_pMapProperty->m_aRangeColour;
      //ASSERT(aRangeColour.GetSize() == m_lbLayers.GetCount());

      int i = 0; for (i = 0; i < m_aRangeSchemes.GetSize(); i++)
      {      
         m_cbColourScheme.AddString(m_aRangeSchemes[i].GetName());
      };


      for (i = 0; i < m_aRangeSchemes.GetSize(); i++)
      {
         int j = 0; for (j = 0; j < aRangeColour.GetSize(); j++)
         {
            if (!(aRangeColour[j] == GetColourRange(j, i))) break;            
         }
         if (j == aRangeColour.GetSize())
         {
            m_cbColourScheme.SetCurSel(i);
         }
      };
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Display menu for loading or saving scheme
//

void CPageMapLayerProp::OnSchememenu() 
{
   int index = m_cbAutoStyle.GetCurSel();
   DWORD dw = m_cbAutoStyle.GetItemData(index);   

   // Get menu
   
   CMenu menu;   

   if (dw == rangecolor)
   {
      menu.LoadMenu(IDR_ADVANCED);
   } else
   {
      menu.LoadMenu(IDR_SCHEME);   
   };

   CMenu* pMenu = menu.GetSubMenu(0);

   // Display

   CRect rect;
   m_pbSchemeMenu.GetWindowRect(&rect);

   pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                         rect.right, rect.top, this);
	
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnSchemeSave() 
{
   
   // Add the new scheme to the list of schemes

   DWORD dwStyle = m_cbAutoStyle.GetItemData(m_cbAutoStyle.GetCurSel());

   if (dwStyle == sepcolor || dwStyle == legendvalues)
   {
      CStyleScheme scheme;

      CDlgSaveScheme dlg;
      if (dlg.DoModal() == IDOK)
      {      
         scheme.m_sName = dlg.GetName();

         int i = 0; for (i = 0; i < m_lbLayers.GetCount(); i++)
         {
            CMapStyle mapstyle = m_lbLayers.GetStyle(i);
            scheme.m_aStyle.Add(mapstyle);
         };
      };

      // If there is already a scheme of the same name then replace it

      int i = 0; for (i = 0; i < m_aStyleSchemes.GetSize(); i++)
      {
         if (m_aStyleSchemes[i].m_sName == scheme.m_sName)
         {
            m_aStyleSchemes.RemoveAt(i);
            break;
         }
      }
      m_aStyleSchemes.InsertAt(i,scheme);
   } 
   
   // Colour ranges


   else if (dwStyle == rangecolor)
   {
      UpdateRangeColour();

      CColourRangeArray &colourrange = m_pMapProperty->m_aRangeColour;

      CDlgSaveScheme dlg;
      if (dlg.DoModal() == IDOK)
      {      
         colourrange.m_sName = dlg.GetName();

         // If there is already a scheme of the same name then replace it

         int i = 0; for (i = 0; i < m_aRangeSchemes.GetSize(); i++)
         {
            if (m_aRangeSchemes[i].m_sName == colourrange.m_sName)
            {
               m_aRangeSchemes.RemoveAt(i);
               break;
            }
         }
         // Add to list

         m_aRangeSchemes.InsertAt(0, colourrange);
      };
   }

   // Save changes and refresh list

   SaveSchemes();
   InitSchemes();
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::SaveSchemes()
{
   // Now save the new list of schemes

   BeginWaitCursor();
   CXMLFile xmlfile;

   m_aStyleSchemes.AsXML(xmlfile);
   m_aRangeSchemes.AsXML(xmlfile);

   xmlfile.Write(BDGetAppPath() + SCHEMEFILE);
   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnSchemeDelete() 
{
   CString sScheme, s;
   CStyleScheme scheme;


   // Retrieve name of scheme   

   m_cbColourScheme.GetWindowText(sScheme);

   DWORD dwStyle = m_cbAutoStyle.GetItemData(m_cbAutoStyle.GetCurSel());
   if (dwStyle == sepcolor || dwStyle == legendvalues)
   {
      int i = 0; for (i = 0; i < m_aStyleSchemes.GetSize(); i++)
      {
         if (m_aStyleSchemes[i].m_sName == sScheme)
         {
            if (!m_aStyleSchemes[i].m_bSystem)
            {
               m_aStyleSchemes.RemoveAt(i);
               SaveSchemes();
               InitSchemes();
            }
         }
      }            
   } 
   else if (dwStyle == rangecolor)
   {
      int i = 0; for (i = 0; i < m_aRangeSchemes.GetSize(); i++)
      {
         if (m_aRangeSchemes[i].m_sName == sScheme)
         {
            m_aRangeSchemes.RemoveAt(i);
            SaveSchemes();
            InitSchemes();
            break;
         }
      }            
   }
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapLayerProp::OnAdvanced() 
{
	CDlgColourRange dlg(m_pMapLayer, m_pMapProperty);

   if (dlg.DoModal() == IDOK)
   {
      RangeColour(TRUE);      
   }	
}
