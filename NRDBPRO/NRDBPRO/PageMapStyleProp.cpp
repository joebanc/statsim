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
#include "PageMapStyleProp.h"
#include "sheetmapprop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageMapStyleProp property page

IMPLEMENT_DYNCREATE(CPageMapStyleProp, CPropertyPage)

CPageMapStyleProp::CPageMapStyleProp()
{
   m_bCoord = FALSE;
   m_bMapLines = FALSE;
}

CPageMapStyleProp::CPageMapStyleProp(CMapLayer* pMapLayer, CMapProperties* pMapProperty) 
   : CPropertyPage(CPageMapStyleProp::IDD)
{
   m_pMapLayer = pMapLayer;
   m_pMapProperty = pMapProperty;

	//{{AFX_DATA_INIT(CPageMapStyleProp)	
	//}}AFX_DATA_INIT
}

CPageMapStyleProp::~CPageMapStyleProp()
{
}

void CPageMapStyleProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMapStyleProp)	
	DDX_Control(pDX, IDC_SYMSIZE, m_cbSymSize);
	DDX_Control(pDX, IDC_SYMBOL_MENU, m_pbSymbolMenu);
   DDX_Control(pDX, IDC_LINESTYLE, m_cbLineStyle);
   DDX_Control(pDX, IDC_SYMBOL, m_cbSymbol);
   DDX_Control(pDX, IDC_FILLSTYLE, m_cbPattern);	
	DDX_Control(pDX, IDC_BESTFITSYM, m_ckBestFitSym);   
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_COLORLINE, m_pbColourLine);
	DDX_Control(pDX, IDC_COLORFILL, m_pbColourFill);
	DDX_Control(pDX, IDC_AUTOFILL, m_ckAutoFill);
   DDX_Control(pDX, IDC_AUTOLINE, m_ckAutoLine);	
   
}


BEGIN_MESSAGE_MAP(CPageMapStyleProp, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMapStyleProp)	
	ON_CBN_SELCHANGE(IDC_LINESTYLE, OnSelchangeLinestyle)
	ON_CBN_SELCHANGE(IDC_FILLSTYLE, OnSelchangeFillstyle)
	ON_BN_CHANGED(IDC_COLORLINE, OnColorline)
	ON_BN_CHANGED(IDC_COLORFILL, OnColorfill)
   ON_BN_CLICKED(IDC_AUTOFILL, OnAutoColour)
	ON_BN_CLICKED(IDC_AUTOLINE, OnAutoColour)
	ON_CBN_SELCHANGE(IDC_SYMBOL, OnSelchangeSymbol)
	ON_CBN_SELCHANGE(IDC_SYMSIZE, OnSelchangeSymsize)
	//}}AFX_MSG_MAP

   ON_BN_CLICKED(IDC_SYMBOL_MENU, OnSymbolMenu)
	ON_COMMAND(ID_SYMBOLS_ADD, OnSymbolsAdd)
	ON_COMMAND(ID_SYMBOLS_DELETE, OnSymbolsDelete)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMapStyleProp message handlers

BOOL CPageMapStyleProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Scan the map objects to determine which properties to display

   m_bCoord = FALSE;
   m_bMapLines = FALSE;
   int i = 0; for (i = 0; i < m_pMapLayer->GetSize(); i++)
   {
      CMapLayerObj* pMapLayerObj = (CMapLayerObj*)m_pMapLayer->GetAt(i);
      if (pMapLayerObj->GetDataType() == BDCOORD) m_bCoord = TRUE;
      if (pMapLayerObj->GetDataType() == BDMAPLINES) m_bMapLines = TRUE;
   }

   // Initialise controls

	m_cbSymbol.Initialise(m_pMapLayer->GetMapProp());
   m_cbLineStyle.Initialise(m_pMapLayer->GetLineStyle(), m_pMapLayer->GetLineWidth());   

   // Initialise symbol size

   for (i = 1; i <= 10; i++)
   {
      CString s;
      s.Format("%d", i);
      int index = m_cbSymSize.AddString(s);
      m_cbSymSize.SetItemData(index, i);

      if (i == (int)m_pMapLayer->GetSymSize()) m_cbSymSize.SetCurSel(index);
   }
   if (m_cbSymSize.GetCurSel() == CB_ERR) m_cbSymSize.SetCurSel(0);
   
   // Set best fit check

   m_ckBestFitSym.SetCheck(m_pMapLayer->GetBestFitSym());

   GetDlgItem(IDS_FILLSTYLE)->ShowWindow(SW_SHOW);   

   m_cbPattern.Initialise(m_pMapLayer->GetPattern(), m_pMapLayer->GetHatch());

   // Set the default colors
   
   m_pbColourLine.SetColour(m_pMapLayer->GetColourLine());
   m_pbColourFill.SetColour(m_pMapLayer->GetColourFill());

   // Initialise menu button

   m_bitmapPopup.LoadBitmap(IDB_POPUPMENU);
   m_pbSymbolMenu.SetBitmap(m_bitmapPopup);

  // Determine whether to use the automatic color or to select

   OnAutoColour();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines whether lines, symbols and fills are to use a specific color
// or a selected one
//

void CPageMapStyleProp::OnAutoColour() 
{   
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageMapStyleProp::OnSetActive() 
{
   // Set this as the default page

   CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
   if (pSheet->IsKindOf(RUNTIME_CLASS(CSheetMapProp)))
   {
      pSheet->m_nDefaultPage = CSheetMapProp::style;
   };

   // The auto buttons are only enabled for certain styles

   BOOL bEnable = m_pMapProperty->m_bAutoColour;

   OnAutoColour();

   // Enable/Disable controls as appropriate

   BOOL bSepColour = m_pMapProperty->m_nSepColour || m_pMapProperty->m_bRangeColour;

   m_cbLineStyle.EnableWindow((m_bMapLines|m_bCoord));	            

   m_cbPattern.EnableWindow(((m_bMapLines && m_pMapProperty->m_bPolygon) || m_bCoord));
   m_pbColourFill.EnableWindow(m_cbPattern.IsWindowEnabled());   
   
	m_cbSymbol.EnableWindow(m_bCoord);   
   m_cbSymSize.EnableWindow(m_bCoord);
   m_pbSymbolMenu.EnableWindow(m_bCoord && !bSepColour);
   m_ckBestFitSym.EnableWindow(m_bCoord);  
   	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////

BOOL CPageMapStyleProp::OnKillActive() 
{   
   int iLine = m_cbLineStyle.GetCurSel();
   int iSym = m_cbSymbol.GetCurSel();
   int iSymSize = m_cbSymSize.GetCurSel();

   if (iSym != CB_ERR && iLine != CB_ERR)
   {
      (CMapStyle&)*m_pMapProperty = m_cbSymbol.GetStyle(iSym);      

      // Get size afterwards as this overides the default size

      m_pMapProperty->m_dSymSize = m_cbSymSize.GetItemData(iSymSize);

      m_pMapProperty->m_nLineStyle = m_cbLineStyle.GetStyle(iLine);
      m_pMapProperty->m_nLineWidth = m_cbLineStyle.GetWidth(iLine);
      

      m_pMapProperty->m_bBestFitSym = m_ckBestFitSym.GetCheck();

      // Support for pattern fills

      int iPatt = m_cbPattern.GetCurSel();
      if (iPatt != CB_ERR)
      {
         m_pMapProperty->m_nPattern = m_cbPattern.GetPattern(iPatt);
         m_pMapProperty->m_nHatch = m_cbPattern.GetHatch(iPatt);
      } else
      {
         return FALSE;
      }

      // Retrieve colors

      m_pMapProperty->m_crLine =  m_pbColourLine.GetColour();
      m_pMapProperty->m_crFill =  m_pbColourFill.GetColour();

      // Save any changes made to the layer page

      CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
      if (IsWindow(pSheet->m_pMapLayerProp->GetSafeHwnd()))
      {
         pSheet->m_pMapLayerProp->OnKillActive();
      };
            
	   return CPropertyPage::OnKillActive();
   };
   return FALSE;		
}

/////////////////////////////////////////////////////////////////////////////////

void CPageMapStyleProp::OnOK() 
{
   m_pMapLayer->GetMapProp() = *m_pMapProperty;
	
	CPropertyPage::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
//
// For styles for which each mapobject has a different style, apply the new
// style when it changes
//

void CPageMapStyleProp::OnSelchangeLinestyle() 
{
   BOOL bSepColour = m_pMapProperty->m_nSepColour || m_pMapProperty->m_bRangeColour;   

   // Apply to separate colour selection
   if (bSepColour)
   {
       // Get access to the layer page
      CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
      if (IsWindow(pSheet->m_pMapLayerProp->GetSafeHwnd()))
      {
         CListBoxStyle &lbLayers = pSheet->m_pMapLayerProp->GetLBLayers();

         // Get the line style
         int iLine = m_cbLineStyle.GetCurSel();      

         // Set it to the selected layers
         int i = 0; for (i = 0; i < lbLayers.GetCount(); i++)
         {
            if (lbLayers.GetSel(i) || lbLayers.GetSelCount() == 0)
            {
               CMapStyle style = lbLayers.GetStyle(i);
               style.m_nLineStyle = m_cbLineStyle.GetStyle(iLine);
               style.m_nLineWidth = m_cbLineStyle.GetWidth(iLine);
               lbLayers.SetStyle(i, style);               
            };
         };
      };
   }
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapStyleProp::OnSelchangeFillstyle() 
{
	BOOL bSepColour = m_pMapProperty->m_nSepColour || m_pMapProperty->m_bRangeColour;   

   // Apply to separate colour selection
   if (bSepColour)
   {
       // Get access to the layer page
      CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
      if (IsWindow(pSheet->m_pMapLayerProp->GetSafeHwnd()))
      {
         CListBoxStyle &lbLayers = pSheet->m_pMapLayerProp->GetLBLayers();

         // Get the line style
         int iLine = m_cbPattern.GetCurSel();      

         // Set it to the selected layers
         int i = 0; for (i = 0; i < lbLayers.GetCount(); i++)
         {
            if (lbLayers.GetSel(i) || lbLayers.GetSelCount() == 0)
            {
               CMapStyle style = lbLayers.GetStyle(i);
               style.m_nPattern = m_cbPattern.GetPattern(iLine);
               style.m_nHatch = m_cbPattern.GetHatch(iLine);
               lbLayers.SetStyle(i, style);               
            };
         };
      };
   }	
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapStyleProp::OnSelchangeSymbol() 
{
	BOOL bSepColour = m_pMapProperty->m_nSepColour || m_pMapProperty->m_bRangeColour;   

   // Apply to separate colour selection
   if (bSepColour)
   {
       // Get access to the layer page
      CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
      if (IsWindow(pSheet->m_pMapLayerProp->GetSafeHwnd()))
      {
         CListBoxStyle &lbLayers = pSheet->m_pMapLayerProp->GetLBLayers();

         // Get the symbol
         int iSym = m_cbSymbol.GetCurSel();      

         // Set it to the selected layers
         int i = 0; for (i = 0; i < lbLayers.GetCount(); i++)
         {
            if (lbLayers.GetSel(i) || lbLayers.GetSelCount() == 0)
            {
               CMapStyle style = lbLayers.GetStyle(i);
               style.m_nSymbol = m_cbSymbol.GetStyle(iSym).m_nSymbol;
               lbLayers.SetStyle(i, style);               
            };
         };
      };
   }			
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapStyleProp::OnSelchangeSymsize() 
{
	BOOL bSepColour = m_pMapProperty->m_nSepColour || m_pMapProperty->m_bRangeColour;   

   // Apply to separate colour selection
   if (bSepColour)
   {
       // Get access to the layer page
      CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
      if (IsWindow(pSheet->m_pMapLayerProp->GetSafeHwnd()))
      {
         CListBoxStyle &lbLayers = pSheet->m_pMapLayerProp->GetLBLayers();

         // Get the symbol
         int iSym = m_cbSymSize.GetCurSel();      

         // Set it to the selected layers
         int i = 0; for (i = 0; i < lbLayers.GetCount(); i++)
         {
            if (lbLayers.GetSel(i) || lbLayers.GetSelCount() == 0)
            {
               CMapStyle style = lbLayers.GetStyle(i);
               style.m_dSymSize = m_cbSymSize.GetItemData(iSym);
               lbLayers.SetStyle(i, style);               
            };
         };
      };
   }				
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapStyleProp::OnColorline() 
{
   BOOL bSepColour = m_pMapProperty->m_nSepColour || m_pMapProperty->m_bRangeColour;   
   if (bSepColour)
   {
       // Get access to the layer page
      CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
      if (IsWindow(pSheet->m_pMapLayerProp->GetSafeHwnd()))
      {
         CListBoxStyle &lbLayers = pSheet->m_pMapLayerProp->GetLBLayers();      

         // Set it to the selected layers
         int i = 0; for (i = 0; i < lbLayers.GetCount(); i++)
         {
            if (lbLayers.GetSel(i) || lbLayers.GetSelCount() == 0)
            {
               CMapStyle style = lbLayers.GetStyle(i);
               style.m_crLine = m_pbColourLine.GetColour();
               lbLayers.SetStyle(i, style);               
            };
         };
      };
   }
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapStyleProp::OnColorfill() 
{   
   BOOL bSepColour = m_pMapProperty->m_nSepColour || m_pMapProperty->m_bRangeColour;   
   if (bSepColour)
   {
       // Get access to the layer page
      CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
      if (IsWindow(pSheet->m_pMapLayerProp->GetSafeHwnd()))
      {
         CListBoxStyle &lbLayers = pSheet->m_pMapLayerProp->GetLBLayers();      

         // Set it to the selected layers
         int i = 0; for (i = 0; i < lbLayers.GetCount(); i++)
         {
            if (lbLayers.GetSel(i) || lbLayers.GetSelCount() == 0)
            {
               CMapStyle style = lbLayers.GetStyle(i);
               style.m_crFill = m_pbColourFill.GetColour();
               lbLayers.SetStyle(i, style);               
            };
         };         
      };
   }
}


///////////////////////////////////////////////////////////////////////////////
//
// Display symbols menu
//

void CPageMapStyleProp::OnSymbolMenu() 
{
   CMenu menu;
   
   menu.LoadMenu(IDR_SYMBOLS);   

   CMenu* pMenu = menu.GetSubMenu(0);

   // Display

   CRect rect;
   m_pbSymbolMenu.GetWindowRect(&rect);

   pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                         rect.right, rect.top, this);
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Add a new custom symbol
//

void CPageMapStyleProp::OnSymbolsAdd() 
{
   if (m_cbSymbol.AddSymbol())
   {
      // Reinitialise list

      int iSym = m_cbSymbol.GetCurSel();
      CMapStyle mapstyle;
      mapstyle = m_cbSymbol.GetStyle(iSym);      
      m_cbSymbol.Initialise(mapstyle);
   };	
}

///////////////////////////////////////////////////////////////////////////////
//
// Delete an existing custom symbol
//

void CPageMapStyleProp::OnSymbolsDelete() 
{
   if (m_cbSymbol.DeleteSymbol())
   {
      // Reinitialise list

	   int iSym = m_cbSymbol.GetCurSel();
      CMapStyle mapstyle;
      mapstyle = m_cbSymbol.GetStyle(iSym);      
      m_cbSymbol.Initialise(mapstyle);
   };	
}



