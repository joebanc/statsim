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
#include "nrdbpro.h"

#include "docmap.h"
#include "dlgmaplayers.h"
#include "viewlegend.h"
#include "viewmap.h"
#include "dlgprogress.h"
#include "framemap.h"
#include "shapefile.h"
#include "dlgscale.h"
#include "dlgprojection.h"
#include "dlglayout.h"
#include "projctns.h"
#include "importmaplines.h"
#include "dlgmapgrid.h"
#include "dlglayout.h"
#include "viewlocator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocMap

IMPLEMENT_DYNCREATE(CDocMap, CDocument)

BEGIN_MESSAGE_MAP(CDocMap, CDocument)
	//{{AFX_MSG_MAP(CDocMap)	
	ON_COMMAND(ID_VIEW_LAYERS, OnViewLayers)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
   ON_COMMAND(ID_SCALE, OnMapScale)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateMapZoom)
	ON_COMMAND(ID_EXPORT_SHAPEFILE, OnExportShapefile)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_SHAPEFILE, OnUpdateExportShapefile)
	ON_COMMAND(ID_TOOLS_PROJECTIONS, OnToolsProjections)	
	ON_UPDATE_COMMAND_UI(ID_MAP_LAYOUT, OnUpdateMapLayout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMNORMAL, OnUpdateMapZoom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateMapZoom)		
	ON_UPDATE_COMMAND_UI(ID_MAP_GRID, OnUpdateMapGrid)
	//}}AFX_MSG_MAP
   ON_COMMAND(ID_MAP_GRID, OnMapGrid)
   ON_COMMAND(ID_MAP_LAYOUT, OnMapLayout)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocMap construction/destruction

CDocMap::CDocMap()
{
   // Initialise    

	// TODO: add one-time construction code here

   m_nScale = 0;

}

CDocMap::~CDocMap()
{
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDocMap::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)   
 
   CString sTitle = BDGetSettings().m_Organization;
   sTitle.TrimLeft();

   if (sTitle != "")
   {
      SetTitle(BDGetSettings().m_Organization);
   } else
   {
      SetTitle(CString(BDGetDataSource()).Mid(4));
   }   
   
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDocMap serialization

void CDocMap::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDocMap diagnostics

#ifdef _DEBUG
void CDocMap::AssertValid() const
{
	CDocument::AssertValid();
}

void CDocMap::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocMap commands

BOOL CDocMap::CanCloseFrame(CFrameWnd* pFrame) 
{
   if (AfxMessageBox(IDS_SAVECHANGES, MB_YESNO) == IDYES)
   {
      OnFileSaveAs();
   }
   return TRUE;

}


/////////////////////////////////////////////////////////////////////////////

void CDocMap::OnViewLayers() 
{
   if (this != NULL)
   {        
      CDlgMapLayers dlg(&m_aMapObject, this);
      if (dlg.DoModal() == IDOK)
      {
         // Check that the map fits on the screen
      
        GetViewMap()->CheckExtent(&m_aMapObject);
      }      
   };
}


/////////////////////////////////////////////////////////////////////////////

CViewLegend* CDocMap::GetViewLegend()
{
   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      CViewLegend* pView = (CViewLegend*)GetNextView(pos);
      if (pView->IsKindOf(RUNTIME_CLASS(CViewLegend))) 
      {
         return pView;
      };
   }
   ASSERT(FALSE);
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////

CViewMap* CDocMap::GetViewMap()
{
   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      CViewMap* pView = (CViewMap*)GetNextView(pos);
      if (pView->IsKindOf(RUNTIME_CLASS(CViewMap))) 
      {
         return pView;
      };
   }
   ASSERT(FALSE);
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////

CViewLocator* CDocMap::GetViewLocator()
{
   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      CViewLocator* pView = (CViewLocator*)GetNextView(pos);
      if (pView->IsKindOf(RUNTIME_CLASS(CViewLocator))) 
      {
         return pView;
      };
   }
   ASSERT(FALSE);
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////

void CDocMap::Activate()
{
   CFrameMap* pFrame = (CFrameMap*)GetViewMap()->GetParent()->GetParent();
   ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CFrameMap)));

   pFrame->ActivateFrame();   
}

/////////////////////////////////////////////////////////////////////////////

void CDocMap::OnFileSaveAs() 
{   
   CString sFilter = BDString(IDS_NRDBFILE) + "|*.nrm|";
   CImageFile::GetOpenFilterString(sFilter, CImageFile::save);
   
   CFileDialog dlg(FALSE, "nrm", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, 
                   sFilter);

   if (dlg.DoModal() == IDOK)
   {     
      if (dlg.GetFileExt() == "nrm")
      {
         m_aMapObject.Write(dlg.GetPathName());	            
      } else
      {       
	      GetViewMap()->SaveImage(dlg.GetPathName());
      };
   };	   
}

/////////////////////////////////////////////////////////////////////////////

void CDocMap::OnFileOpen() 
{
   Open(NULL);	
}

/////////////////////////////////////////////////////////////////////////////

void CDocMap::Open(LPCSTR sFile)
{
  // Strip quotes

   CString sPath = sFile;
   if (sPath .GetLength() > 0 && sPath[0] == '"')
   {
      int i = sPath.Find('"', 1);
      sPath = sPath.Mid(1, i-1);
   }

   if (sPath != "")
   {
      m_aMapObject.OnLoad(sPath);
   } else
   {
      m_aMapObject.OnLoad(NULL);
   }    
   GetViewMap()->Redraw(); // Forces recalculation of extent

   // Check layers extent

   GetViewMap()->CheckExtent(&m_aMapObject);
}

/////////////////////////////////////////////////////////////////////////////

void CDocMap::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetLayers()->GetSize() > 0);		
}

void CDocMap::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetLayers()->GetSize() > 0);			
}

void CDocMap::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetLayers()->GetSize() > 0);				
}

void CDocMap::OnUpdateMapZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetLayers()->GetSize() > 0);					
}

/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Rounds up a maximum and minium value into 'nice' values e.g. 0.13 to 0.98 
// becomes 0 to 10
//

void CDocMap::DetermineScale(double dMin, double dMax, double *pdScaleMin, double *pdScaleMax, 
                             int* pnSteps, BOOL bLatLon)
{ 
      
 // Scale the values so that they lie between 10 and 1000
 
   if (dMax == dMin)
   {
      if (dMax > 0) dMin = 0;
      else if (dMax < 0) dMax = 0;
      else dMax = 1;
   }

   double dScaleFactor = 1;
   
 // Detetermine the range
 
   double dRange = dMax - dMin;   
     
   while (dRange < 100.0)
   {      
     dMax*=10.0;
     dMin*=10.0;
     dRange*=10.0;
     dScaleFactor*=10.0;
   }; 
     
   while (dRange > 1000.0)
   {     
     dMax /=10.0;
     dMin /=10.0;
     dRange /= 10.0;
     dScaleFactor /= 10.0;
   }; 
     
 // Divide the scale by 4,5 and 6 to determine the most appropriate step size
 // for the scale
   
   int anScale[] = {10,20,25,50,100,200, 250};

 // For latitude/longitude, must be a multiple of 60

   if (bLatLon) 
   {
      anScale[0] = 10;
      anScale[1] = 50; // 15 degrees
      anScale[2] = 0;
   }
   
  // Determine the smallest
  
   double dRangeMin = 1000;
   double dScaleStep = 100;
   
   int i = 0; for (i = 0; i < sizeof(anScale)/sizeof(int) && anScale[i] != 0; i++)
   {   
       ASSERT(anScale[i] != 0);
       double dScaleRange = ceil(dRange / anScale[i]) * anScale[i];      
       if (dRangeMin > dScaleRange && dScaleRange / anScale[i] < 7)
       { 
          dRangeMin = dScaleRange;
          dScaleStep = anScale[i];
       };      
   };               
   
   
  // Determine scale max and min
  
   double dScaleMin = floor(dMin/dScaleStep)*dScaleStep; 
   double dScaleMax = ceil(dMax/dScaleStep)*dScaleStep;
   
   // Remove anomilies like 0.1 to 1.0

   if (dScaleMin / dScaleMax < 0.2 && dScaleMin / dScaleMax > 0) dScaleMin = 0;
   
  // Re-adjust scale factor
      
   ASSERT(dScaleFactor != 0);
       
   *pdScaleMin = dScaleMin / dScaleFactor;
   *pdScaleMax = dScaleMax / dScaleFactor;                

   // Determine step

   if (pnSteps != NULL) *pnSteps = (int)((dScaleMax - dScaleMin) / dScaleStep);
   
   return;
};                

///////////////////////////////////////////////////////////////////////////////

void CDocMap::OnExportShapefile() 
{   

   int i = GetLayers()->GetDefault();   
   if (i < GetLayers()->GetSize())
   {
      CMapLayer* pMapLayer = GetLayers()->GetAt(i);      

      // Determine if any part of the map layer is protected

      int i = 0; for (i = 0; i < pMapLayer->GetSize(); i++)
      {
         CMapLayerObj* pMapObj = pMapLayer->GetAt(i);
         if (pMapObj->GetDataType() == BDMAPLINES)
         {
            CLongLines* pLongLines = (CLongLines*)pMapObj->GetMapObject();
            if (pLongLines->IsProtected())
            {
               AfxMessageBox(pLongLines->GetProtected());
               return;
            }
         }
         
      }

      // Display file save dialog

      CString sFilter = BDString(IDS_SHAPEFILE) + " (" + BDString(IDS_LATLON) + ")|*.shx|";
      if (!BDProjection()->IsLatLon()) sFilter += BDString(IDS_SHAPEFILE) + " (" + BDString(IDS_XY) + ")|*.shp|";

      if (!BDProjection()->IsLatLon()) sFilter += BDString(IDS_POLYLINESFILE) + " (*.txt)|*.txt|";
      sFilter += BDString(IDS_LATLONFILE) + " (*.txt)|*.ll||";

	   CFileDialog dlg(FALSE, "shp", pMapLayer->GetName(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                      sFilter);

      if (dlg.DoModal() == IDOK)
      {               
         BOOL bOK = FALSE;

         // Export to a text file

         BOOL bTextLatLon = dlg.GetFileExt().CompareNoCase("ll") == 0;
         BOOL bTextXY = dlg.GetFileExt().CompareNoCase("txt") == 0;

         if (bTextLatLon || bTextXY)
         {
            // Change extension to .txt

            CString sPath = dlg.GetPathName();
            sPath = sPath.Mid(0, sPath.Find('.')) + ".txt";

            bOK = ExportMapLayer(pMapLayer, sPath, bTextLatLon);
         }         
         else
         {
            BOOL bLatLon = dlg.GetFileExt().CompareNoCase("shx") == 0;
            CString sPath = dlg.GetPathName();
		      sPath.SetAt(sPath.GetLength()-1,'p');


            int nFlag = bLatLon;
            if (pMapLayer->IsPolygon()) nFlag |= CShapeFile::Polygon;
         
            CShapeFile shapefile;
            BeginWaitCursor();
            bOK = shapefile.ExportShapeFile(pMapLayer, sPath, nFlag);            
            EndWaitCursor();
         };

         // Display export results

         if (bOK == TRUE)
         {
            AfxMessageBox(IDS_EXPORTESUCCESS);
         } else if (bOK == FALSE)
         {
            AfxMessageBox(IDS_ERROREXPORT);
         }
      };	      
   }	
}

///////////////////////////////////////////////////////////////////////////////

void CDocMap::OnUpdateExportShapefile(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE;

   // Prevent export when linked to a file

   int i = GetLayers()->GetDefault();   
   if (i < GetLayers()->GetSize())
   {
      CMapLayer* pMapLayer = GetLayers()->GetAt(i);      
      //bEnable = (CString)pMapLayer->GetFileName() == "";

      if (pMapLayer->GetSize() > 0)
      {
         CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(0);
         bEnable = pMapLayerObj->GetDataType() == BDMAPLINES ||
                   pMapLayerObj->GetDataType() == BDCOORD; 
      }     
   };

   pCmdUI->Enable(bEnable);
   
}

///////////////////////////////////////////////////////////////////////////////
//
// void CDocMap::OnMapScale()
//

void CDocMap::OnMapScale()
{

   CDlgScale dlg(m_nScale);
   if (dlg.DoModal() == IDOK)
   {
      m_nScale = dlg.GetScale();
      GetViewMap()->Redraw();
   }

}

///////////////////////////////////////////////////////////////////////////////

void CDocMap::OnToolsProjections() 
{
   CDlgProjection dlg;
   dlg.DoModal();	

   BDProjection()->InitialiseProjection(BDHandle());

   GetViewMap()->InitialiseMapView();
   GetViewMap()->OnViewZoomnormal();
	
}

///////////////////////////////////////////////////////////////////////////////
//
// If closing the document then release the memory for the existing layers
//

void CDocMap::OnCloseDocument() 
{	  

	CDocument::OnCloseDocument();
}

///////////////////////////////////////////////////////////////////////////////

void CDocMap::OnMapLayout() 
{
   CDlgLayout dlg;
   if (dlg.DoModal() == IDOK)
   {      
   }
}

///////////////////////////////////////////////////////////////////////////////

void CDocMap::OnUpdateMapLayout(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetLayers()->GetSize() > 0);			
}

///////////////////////////////////////////////////////////////////////////////

void CDocMap::OnMapGrid() 
{
   CDlgMapGrid dlg(m_aMapObject.GetMapGrid());
   if (dlg.DoModal() == IDOK)
   {
      m_aMapObject.SetMapGrid(dlg.GetMapGrid());
      GetViewMap()->Redraw();
   }
	
}

///////////////////////////////////////////////////////////////////////////////

void CDocMap::OnUpdateMapGrid(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetLayers()->GetSize() > 0);			
	
}
