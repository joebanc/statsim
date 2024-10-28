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
#include <math.h>

#include "nrdb.h"
#include "DlgMapLayers.h"
#include "dlgaddlayer.h"
#include "sheetquery.h"
#include "importmaplines.h"
#include "dlgprogress.h"
#include "viewmap.h"
#include "sheetmapprop.h"
#include "mainfrm.h"
#include "shapefile.h"
#include "comboboxsymbol.h"
#include "dlgaddoverlay.h"
#include "dlgdigitisetype.h"
#include "shapefile.h"
#include "dlgftypeattr.h"
#include "spatial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMapLayers dialog


CDlgMapLayers::CDlgMapLayers(CMapLayerArray* paMapObject, CDocMap* pDoc, 
                             CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapLayers::IDD, pParent)
{

   m_paMapObject = paMapObject;
   m_pDocMap = pDoc;

	//{{AFX_DATA_INIT(CDlgMapLayers)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgMapLayers::~CDlgMapLayers()
{
}

void CDlgMapLayers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMapLayers)
	DDX_Control(pDX, IDC_AUTOUPDATE, m_ckAutoUpdate);
	DDX_Control(pDX, IDC_MAPLAYER, m_lbMapLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMapLayers, CDialog)
	//{{AFX_MSG_MAP(CDlgMapLayers)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN, OnDeltaposUpdown)
	ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)	
	ON_BN_CLICKED(IDC_QUERY, OnQuery)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_LEGEND, OnLegend)
	ON_LBN_DBLCLK(IDC_MAPLAYER, OnDblclkMaplayer)
	ON_BN_CLICKED(IDC_OVERLAYS, OnOverlays)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_LBN_SELCHANGE(IDC_MAPLAYER, OnSelchangeMaplayer)
	ON_BN_CLICKED(IDC_AUTOUPDATE, OnAutoupdate)
	ON_BN_CLICKED(IDC_CBMS, OnCbms)
	//}}AFX_MSG_MAP
  END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMapLayers message handlers

BOOL CDlgMapLayers::OnInitDialog() 
{
	CDialog::OnInitDialog();

   // Initialise tick list box as single selection

   m_lbMapLayer.Init(FALSE);

   BDSetActiveWnd(GetSafeHwnd());

   // Option to switch off auto updating of map (must initialise before calling 'preview report')

   m_ckAutoUpdate.SetCheck(m_pDocMap->GetViewMap()->IsAutoUpdate());


   // First list of layers

   InitLayers(m_paMapObject->GetDefault(), TRUE);

   // Previous reports

   if (BDGetPreviousReport() == CNRDB::maplayer)
   {
      OnAdd();
      OnOK();
   };
   if (BDGetPreviousReport() == CNRDB::mapquery) 
   {
      OnQuery();
      OnOK();
   };

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::InitLayers(int iSel, BOOL bInit)
{
   m_lbMapLayer.ResetContent();

   int i = 0; for (i = 0; i < m_paMapObject->GetSize(); i++)
   {
      int index = m_lbMapLayer.AddString(m_paMapObject->GetAt(i)->GetName());


      m_lbMapLayer.SetCheck(index, m_paMapObject->GetAt(i)->IsVisible());

      if (iSel == i) m_lbMapLayer.SetCurSel(iSel);
   }

   // Disable properties button if no layers

   GetDlgItem(IDC_PROPERTIES)->EnableWindow(m_lbMapLayer.GetCount() > 0);
   GetDlgItem(IDC_DELETE)->EnableWindow(m_lbMapLayer.GetCount() > 0);   
  
   // Redraw map

   if (!bInit)
   {
      m_pDocMap->UpdateAllViews(NULL);   
   };
}

/////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::OnAdd() 
{   
   CDlgAddLayer dlg;  
   CString s;   
   
   if (dlg.DoModal() == IDOK)
   {        
      CDlgProgress dlgProgress(FALSE, this);
      
      CMapLayer* pMapLayer = new CMapLayer(dlg.IsMapLines());

      // Load data or create an empty layer if no features were selected

      if (pMapLayer->Initialise(dlg.GetFType(), dlg.GetFeatures(), 
                            dlg.GetAttr(), dlg.GetColour()) ||
                            dlg.GetFeatures().GetSize() == 0)
      {      
        // Update the list of layers
         
	      m_paMapObject->InsertAt(0, pMapLayer);
         InitLayers(0);
      
         // Set previous report

         BDSetPreviousReport(CNRDB::maplayer);
      } else
      {
         delete pMapLayer;
         
      }
   }           
}


/////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::OnDelete() 
{
   BeginWaitCursor();

   int i = m_lbMapLayer.GetCurSel();
   if (i != LB_ERR)
   {
      CMapLayer* pMapLayer = (CMapLayer*)m_paMapObject->GetAt(i);
      ASSERT(pMapLayer != NULL);

      delete pMapLayer;
      m_paMapObject->RemoveAt(i);

   m_pDocMap->GetViewMap()->m_aSearchObj.RemoveAll();
   }	

   // Update the list of layers

   InitLayers(max(0,i-1));

   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////
//
// Re-order the list of layers
//

void CDlgMapLayers::OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   
	int i = m_lbMapLayer.GetCurSel();
   int iSwap = -1;

   // Determine which items to swap

   if (pNMUpDown->iDelta == -1 && i > 0)
   {
      iSwap = i-1;
      
   }    
   else if (pNMUpDown->iDelta == 1 && i != LB_ERR && i+1 < m_lbMapLayer.GetCount())
   {
      iSwap = i+1;
   };

   // Swap the items
   if (iSwap != -1)
   {
      CMapLayer* pMapLayer = (CMapLayer*)m_paMapObject->GetAt(iSwap);
      m_paMapObject->SetAt(iSwap, m_paMapObject->GetAt(i));
      m_paMapObject->SetAt(i, pMapLayer);

      InitLayers(iSwap);
   };
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::OnProperties() 
{
   int index = m_lbMapLayer.GetCurSel();
   if (index != LB_ERR)
   {
      CMapLayer* pMapLayer = (CMapLayer*)m_paMapObject->GetAt(index);      

      CSheetMapProp dlg(m_paMapObject, pMapLayer, BDString(IDS_MAPPROP));      
      if (dlg.DoModal())
      {
         InitLayers(index);             
      }
   };	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::OnDblclkMaplayer() 
{
   OnProperties();	
}
  
/////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::OnQuery() 
{
   CQuery* pQueryL = NULL;

   int index = m_lbMapLayer.GetCurSel();
   if (index != LB_ERR)
   {
      CMapLayer* pMapLayer = (CMapLayer*)m_paMapObject->GetAt(index);   
      pQueryL = pMapLayer->GetQuery();
   };

   CQuery* pQuery = NULL;
   if (pQueryL != NULL) pQuery = new CQuery(*pQueryL);
   else pQuery = new CQuery;

   CSheetQuery dlg(pQuery, CSheetQuery::Map, BDString(IDS_MAP)); 
   int nRet = dlg.DoModal();

   if (nRet == IDOK)
   {
      CDlgProgress dlgProgress(FALSE, this);

      BeginWaitCursor();
      
      CMapLayer* pMapLayer = new CMapLayer;
      if (pMapLayer->Initialise(pQuery))
      {
         m_paMapObject->InsertAt(0, pMapLayer);
         InitLayers(0);      

         BDSetPreviousReport(CNRDB::mapquery);
      } else
      {
         delete pMapLayer;         

         if (AfxMessageBox(BDString(IDS_QUERYERROR), MB_YESNO) == IDYES)
         {            
            OnQuery();                           
         };         
      }
      EndWaitCursor();
   } 
   else
   {
      delete pQuery;
   }   
}

/////////////////////////////////////////////////////////////////////////////
//
// Create an empty layer to contain a legend and open the map properties
//

void CDlgMapLayers::OnLegend() 
{
   // Create an empty layer

   CMapLayer* pMapLayer = new CMapLayer(FALSE);
   
   // Set the symbol to none

   pMapLayer->GetMapProp().m_nSymbol = CComboBoxSymbol::none;

   // Add to list   

   CSheetMapProp dlg(m_paMapObject, pMapLayer, BDString(IDS_MAPPROP));      
   if (dlg.DoModal() == IDOK)
   {
       m_paMapObject->InsertAt(0, pMapLayer);
      InitLayers(0);
   } else
   {
      delete pMapLayer;
   }
}

/////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::OnClose() 
{  
   OnOK();
}

/////////////////////////////////////////////////////////////////////////////
//
// Store active layer
//

void CDlgMapLayers::OnOK() 
{
   // Set visibility state of layers
   int i = 0; for (i = 0; i < m_lbMapLayer.GetCount(); i++)
   {
      m_paMapObject->GetAt(i)->SetVisible(m_lbMapLayer.IsCheck(i));
   }

   int index = m_lbMapLayer.GetCurSel();
   if (index != LB_ERR)
   {
      m_paMapObject->SetDefault(index);            

      // Update key

   }

   // Set preference for automatic updating

   m_pDocMap->GetViewMap()->SetAutoUpdate(m_ckAutoUpdate.GetCheck());
	
   // Update all views as visibility may have changed

   m_pDocMap->UpdateAllViews(NULL);   
    

	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgMapLayers::OnOverlays() 
{
    CMapLayer* pMapLayer = new CMapLayer;

	CDlgAddOverlay dlg(pMapLayer);
	if (dlg.DoModal() == IDOK)
	{    
       m_paMapObject->InsertAt(0, pMapLayer);

	   InitLayers(0);	         
	} else
	{
		delete pMapLayer;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Ask the user if they wish to digitize points, polylines or polygons.
// Create a new feature type, then start digitizing.
//

void CDlgMapLayers::OnNew() 
{
   int nType = 0;

   // Determine which type to add

   CDlgDigitiseType	dlg;
   if (dlg.DoModal() == IDOK)
   {
      nType = dlg.GetType();

      // Create a new feature type

      int nDataType = 0;
      if (nType == SHPPoint) nDataType = BDCOORD;
      else if (nType == SHPPolygon || nType == SHPPolyLine) nDataType = BDMAPLINES;

      CDlgFTypeAttr dlgF(0,0,nDataType);
      if (dlgF.DoModal() == IDOK)
      {
         // Create a query for this feature type

         CFeatureType ftype;
         CAttrArray aAttr;

         ftype.m_lId = dlgF.GetId();
         BDFeatureType(BDHandle(), &ftype, BDSELECT);
         BDFTypeAttrInit(BDHandle(), ftype.m_lId, &aAttr);
         
         CArrayAttrSel aAttrSel;
         CQueryAttrSel attrsel;

         // Add the feature name

         attrsel.m_lAttr = BDFEATURE;
         attrsel.m_lFType = ftype.m_lId;
         attrsel.m_lDataType = BDFEATURE;
         aAttrSel.Add(attrsel);

         // Add the coordinate or polyline attribute
         int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
         {
            if (aAttr[i]->m_lDataType == nDataType)
            {
               attrsel.m_lAttr = aAttr[i]->m_lAttrId;
               attrsel.m_lDataType = nDataType;
               aAttrSel.Add(attrsel);
               break;
            }
         }         
         
         CLongArray alFeatures; // No features yet

         CQuery* pQuery = new CQuery(dlgF.GetId(), aAttrSel, alFeatures);         

         // Add this ftype as a new map layer

         CMapLayer* pMapLayer = new CMapLayer;

         pMapLayer->Initialise(pQuery); // Will return error as no features yet
         m_paMapObject->InsertAt(0, pMapLayer);
         InitLayers(0);               

         // Set the mode to editing
  
         int nAddType = 0;
         if (nType == SHPPoint) nAddType = CViewMap::points;
         if (nType == SHPPolyLine) nAddType = CViewMap::polyline;
         if (nType == SHPPolygon) nAddType = CViewMap::polygon;

         m_pDocMap->GetViewMap()->SetMode(CViewMap::addnew | nAddType);

         OnOK();
      }     
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// If the visibility has changed then redraw the map layers
//

void CDlgMapLayers::OnSelchangeMaplayer() 
{
   int i = 0; for (i = 0; i < m_paMapObject->GetSize(); i++)
   {
      if (m_paMapObject->GetAt(i)->IsVisible() != m_lbMapLayer.IsCheck(i))
	  {
         m_paMapObject->GetAt(i)->SetVisible(m_lbMapLayer.IsCheck(i)); 
		 m_pDocMap->UpdateAllViews(NULL);   
		 break;
	  }
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Update when checked so takes effect before dialog is closed
//

void CDlgMapLayers::OnAutoupdate() 
{
   m_pDocMap->GetViewMap()->SetAutoUpdate(m_ckAutoUpdate.GetCheck());	
}


///////////////////////////////////////////////////////////////////////////////

//#ifdef CBMS

// define type to hold results
struct CHealth{long lId; double dValue;};

void CDlgMapLayers::OnCbms() 
{
	CAttrArray aAttr;
	CDlgProgress dlgProgress(FALSE, this);
	int iRow = 0;
	BOOL bOK = TRUE;
	BOOL bFound;
	CString s;
	
	CArray <CHealth, CHealth> aHealth;
	CHealth health;
      
	// Create a new map layer and add it to the list

	CMapLayer* pMapLayer = new CMapLayer(TRUE);
	pMapLayer->GetMapProp().m_sName = "% HH w/ malnourished children 0-5";
    pMapLayer->GetMapProp().m_crFill = RGB(220,212,255);
          
	m_paMapObject->InsertAt(0, pMapLayer);
    InitLayers(0);

	// Retrieve data on malnourished children

    aAttr.m_lFType = 12;
	bFound = BDAttribute(BDHandle(), &aAttr, BDGETINIT);
	while (bFound)
	{
        health.lId = aAttr.m_lFeature;
		int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
		{
			if (aAttr[i]->m_lAttrId == 4) 
			{
				health.dValue = *aAttr[i]->GetDouble();
				break;
			}
		}
		aHealth.Add(health);
		dlgProgress.SetPercent((iRow++*100l)/42000);
		bFound = BDGetNext(BDHandle());
	}
	BDEnd(BDHandle());

	// Retrieve the barangay data from the database
   
	iRow = 0;
	aAttr.m_lFType = 6;
	bFound = BDAttribute(BDHandle(), &aAttr, BDGETINIT);
	while (bFound)
	{
		// Create map object

		CMapLayerObj* pMapObj = new CMapLayerObj;
		pMapLayer->Add(pMapObj);
		
   // Determine if there is a corresponding value (optimise thru query / sorting)

	  pMapObj->SetValue(AFX_RFX_DOUBLE_PSEUDO_NULL);
	  int j = 0; for (j = 0; j < aHealth.GetSize(); j++)
	  {
		  if (aAttr.m_lFeature == aHealth[j].lId)
		  {
			  pMapObj->SetValue(aHealth[j].dValue);
			  s.Format("%.0lf",aHealth[j].dValue);
			  pMapObj->SetText(s);
			  break;
		  }
	  }

		// Extract map data from returned attribute array

	  for (j = 0; j < aAttr.GetSize() && bOK; j++)
      {
          CAttribute* pAttr = aAttr[j];

          
           // Set the date associated with the object

           pMapObj->SetDate(aAttr.m_lDate);

           if (pAttr->GetDataType() == BDMAPLINES)
           {                        

              CSpatialPoly* pMapLines = new CSpatialPoly(*pAttr->GetLongBinary());

             
              if (pMapLines != NULL)
              {
                 pMapObj->SetMapObject(pMapLines);                        
                 pMapObj->SetDataType(BDMAPLINES);

              } else
              {
                 AfxMessageBox(BDString(IDS_OUTOFMEMORY));
				 bOK = FALSE;
              }
              break;
           }

		 };

		dlgProgress.SetPercent((iRow++*100l)/42000);
		bFound = BDGetNext(BDHandle());
	}
	BDEnd(BDHandle());

    	
}
//#endif