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
#include "ShapeFile.h"
#include "projctns.h"
#include "maplayer.h"
#include "dlgprogress.h"
#include "spatial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

inline void Swap(double &A, double& B)
{
   double C = A;
   A = B;
   B = C;
}

inline void Swap(int &A, int& B)
{
   int C = A;
   A = B;
   B = C;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShapeFile::CShapeFile()
{
   m_pFileIn = NULL;
   m_pFileOut = NULL;
   m_bLatLon = FALSE;
}

CShapeFile::~CShapeFile()
{

}

//////////////////////////////////////////////////////////////////////

CPolyLine::CPolyLine()
{
   int i = 0; for (i = 0; i < 4; i++ ) m_dBox[i] = 0;
   m_nParts = 0;
   m_nPoints = 0;
}

///////////////////////////////////////////////////////////////////////////////

void CShapeFile::fwritex(void* p, size_t size, DWORD n, FILE* pFile)
{
   if (fwrite(p, size, n, pFile) != n)
   {
      AfxThrowUserException();
   }
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::ImportShapeFile(CLongLines* pMapLines, LPCSTR sFileName)
{
   CMainHeader mainheader;
   CCoordArray aCoord;
   return ImportShapeFile(pMapLines, &aCoord, mainheader, sFileName, PolyLine);
}

///////////////////////////////////////////////////////////////////////////////

int CShapeFile::ImportShapeFileType(LPCSTR sFileName)
{
   CMainHeader mainheader;
   CLongLines maplines;
   CCoordArray aCoord;

   BOOL bOK = ImportShapeFile(&maplines, &aCoord, mainheader, sFileName, Header);
   if (bOK) return mainheader.m_nShapeType;
   else return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Import information from ESRI shapefile, Nb. that only polylines and
// polygons will be supported
//

BOOL CShapeFile::ImportShapeFile(CLongLines* pMapLines, CCoordArray* pCoords, CMainHeader& mainheader, LPCSTR sFileName, int iFlag)
{  
   BOOL bOK = TRUE;   
   CRecordHeader recordheader;
   DWORD nShape;   
   BOOL bPolyLine = FALSE;
   int nBytes = sizeof(CMainHeader);
  
   // Open the file as binary

   FILE* pFile = fopen(sFileName, "rb");
   if (pFile != NULL)
   {
      // Read the file header

      if (fread(&mainheader, sizeof(CMainHeader),1,pFile) == 1)
      {
         mainheader.m_nFileCode = ReverseBytes(mainheader.m_nFileCode);
         mainheader.m_nFileLength = ReverseBytes(mainheader.m_nFileLength);

         // Determine if lat/long or coordinates

         if (mainheader.m_dXMin >= -180 && mainheader.m_dXMax <= 180 &&
            mainheader.m_dYMax <= 90 && mainheader.m_dYMin >= -90)
         {
            iFlag |= LatLon;
         }

         
         // Read the record header

         BDProgressRange(0, mainheader.m_nFileLength);

         while (bOK && fread(&recordheader, sizeof(CRecordHeader),1,pFile) == 1 && 
               !(iFlag & Header))
         {
         // Read the shape type
            if (fread(&nShape,sizeof(nShape),1,pFile) == 1)
            {
               // Import only polylines

               if (nShape == SHPPolyLine || nShape == SHPPolygon)
               {
                  bPolyLine = TRUE;
                  bOK = ImportPolyLine(pMapLines, pFile, iFlag & LatLon);
               } 

               else if (nShape == SHPPoint)
               {
                  bOK = ImportPoints(pCoords, pFile, iFlag & LatLon);
               }
               // Skip the record

               else
               {
				   // Handles corrupt shapefiles

				   if (nShape != 0) bOK = FALSE;
				   				   
                  ASSERT(nShape >= 0 && nShape <= 31);
                  int nContentLength = ReverseBytes(recordheader.m_nContentLength);
                  int i = 0; for (i = 0; bOK && i < nContentLength-2; i++)
                  {
				     WORD word;
                     if (fread(&word,sizeof(word),1,pFile) != 1)
                     {
                        bOK = FALSE;
                     }
                  }
               }

               // Update progress bar
               nBytes += ReverseBytes(recordheader.m_nContentLength);
               BDProgressPos(nBytes);

            } else
            {
               bOK = FALSE;
            }
         }
      } else
      {
         bOK = FALSE;
      }
   }
   if (pFile != NULL) fclose(pFile);

   if (bOK && !bPolyLine && (iFlag & PolyLine) && !(iFlag & Points))
   {
      AfxMessageBox(BDString(IDS_SHAPEFILENOPOLYLINES));
      bOK = FALSE;
   }

   BDProgressPos(0);

   return bOK;
};

///////////////////////////////////////////////////////////////////////////////
//
// Import a shapefile with each record as a separate layer, this should overcome the
// problem of reallocating memory all the time for large files
//

BOOL CShapeFile::ImportShapeFile(CMapLayer* pMapLayer, LPCSTR sFileName)
{
   BOOL bOK = TRUE;   
   CRecordHeader recordheader;
   CMainHeader mainheader;
   DWORD nShape;   
   int nBytes = sizeof(CMainHeader);
   int iFlag = 0;
   BOOL bCancel = FALSE;

   // Open the file as binary

   FILE* pFile = fopen(sFileName, "rb");
   if (pFile != NULL)
   {
      // Read the file header

      if (fread(&mainheader, sizeof(CMainHeader),1,pFile) == 1)
      {
         mainheader.m_nFileCode = ReverseBytes(mainheader.m_nFileCode);
         mainheader.m_nFileLength = ReverseBytes(mainheader.m_nFileLength);

         // Determine if lat/long or coordinates

         if (mainheader.m_dXMin >= -180 && mainheader.m_dXMax <= 180 &&
            mainheader.m_dYMax <= 90 && mainheader.m_dYMin >= -90)
         {
            iFlag |= LatLon;
         }

         
         // Read the record header

         BDProgressRange(0, mainheader.m_nFileLength);

         while (bOK && fread(&recordheader, sizeof(CRecordHeader),1,pFile) == 1 && 
               !(iFlag & Header) && !bCancel)
         {
         // Read the shape type
            if (fread(&nShape,sizeof(nShape),1,pFile) == 1)
            {
			     CMapLayerObj* pMapLayerObj = new CMapLayerObj;

               // Import only polylines

               if (nShape == SHPPolyLine || nShape == SHPPolygon)
               {                  
				      CSpatialPoly *pSpatialPoly = new CSpatialPoly;
				      pMapLayerObj->SetMapObject(pSpatialPoly);                        
                  pMapLayerObj->SetDataType(BDMAPLINES);

                  CRectDbl rect;
                  bOK = ImportPolyLine(pSpatialPoly, pFile, iFlag & LatLon, &rect);
                  pSpatialPoly->Initialise();
               } 

               else if (nShape == SHPPoint)
               {                  
                  CCoordArray aCoord;
                  bOK = ImportPoints(&aCoord, pFile, iFlag & LatLon);

				      CCoord* pCoord = new CCoord(aCoord[0]); 
				      pMapLayerObj->SetMapObject(pCoord);
				      pMapLayerObj->SetDataType(BDCOORD);

               }
               // Skip the record

               else
               {
				   // Handles corrupt shapefile

                  if (nShape != 0)
				  {
					  bOK = FALSE;
				  } else if (mainheader.m_nShapeType == SHPPolyLine || mainheader.m_nShapeType == SHPPolygon)
				  {
                     CSpatialPoly* pSpatialPoly = new CSpatialPoly;
				         pMapLayerObj->SetMapObject(pSpatialPoly);                        
                     pMapLayerObj->SetDataType(BDMAPLINES);
				  } else if (mainheader.m_nShapeType == SHPPoint)
				  {
                 CCoord* pCoord = new CCoord; 
				     pMapLayerObj->SetMapObject(pCoord);
				     pMapLayerObj->SetDataType(BDCOORD);
				  }
				   				   
                  ASSERT(nShape >= 0 && nShape <= 31);
                  int nContentLength = ReverseBytes(recordheader.m_nContentLength);
                  int i = 0; for (i = 0; bOK && i < nContentLength-2; i++)
                  {
				     WORD word;   
                     if (fread(&word,sizeof(word),1,pFile) != 1)
                     {
                        bOK = FALSE;
                     }
                  }
               }

               // Update progress bar
               nBytes += ReverseBytes(recordheader.m_nContentLength);
               if (!BDProgressPos(nBytes))
               {
                  bCancel = TRUE;
               }

			   // Add to the array of map objects
			   			                 
			   pMapLayer->Add(pMapLayerObj);

            } else
            {
               bOK = FALSE;
            }
         }
      } else
      {
         bOK = FALSE;
      }
   }
   if (pFile != NULL) fclose(pFile);   

   // If layer is a polygon then ensure that all polygons are closed

   if (bOK)
   {       
      if (mainheader.m_nShapeType == SHPPolygon)
      {
         PolylineToPolygon(pMapLayer);          
      }   

   }

   if (!bOK && !bCancel)
   {
      AfxMessageBox(BDString(IDS_INVALIDSHAPEFILE));
      bOK = FALSE;
   }


   BDProgressPos(0);

   return bOK && !bCancel;

}

///////////////////////////////////////////////////////////////////////////////
//
// Import polylines or the format
//

BOOL CShapeFile::ImportPolyLine(CLongLines* pMapLines, FILE* pFile, BOOL bLatLon, CRectDbl* pRect)
{   
   CPolyLine polyline;
   BOOL bOK = TRUE;
   CArray <DWORD,DWORD> anParts;
   CArray <double,double> adPoints;
   CLongCoord coord;

   // Read header

   if (fread(&polyline, sizeof(CPolyLine), 1, pFile))
   {
      // Assign space for data

      anParts.SetSize(polyline.m_nParts); 
      adPoints.SetSize(polyline.m_nPoints*2);	  

      // Read data

      if (fread(anParts.GetData(),sizeof(int),polyline.m_nParts, pFile) == polyline.m_nParts &&
          fread(adPoints.GetData(), sizeof(double)*2,polyline.m_nPoints, pFile) == polyline.m_nPoints)
      {  
         // Add a last point to the list of points showing the last array

         anParts.Add(polyline.m_nPoints);

		 int k = pMapLines->GetSize();		
		 pMapLines->SetSize(pMapLines->GetSize() + polyline.m_nPoints+polyline.m_nParts);

		 // Preallocate the size of the array 
		   		     
         for (DWORD i = 0; i < polyline.m_nParts; i++)
         {			
            for (DWORD j = anParts[i]; j < anParts[i+1]; j++)
            {               
               double dX = adPoints[j*2];
               double dY = adPoints[j*2+1];

			      if (bLatLon)
               {
			         BDProjection()->LatLonToTransMercator(dY, dX, &coord.x, &coord.y);
               } else
               {
                  coord.x = (long)(dX + 0.5);
                  coord.y = (long)(dY + 0.5);
               }
				                 			   
			   pMapLines->SetAt(k++, coord);
            }            
            coord.SetNull();
            if (i+1 == polyline.m_nParts) coord.SetEOL();
			pMapLines->SetAt(k++, coord);            
         }         
      } 
      else
      {
         bOK = FALSE;
      }       
   } else
   {
      bOK = FALSE;
   }

   // Optimization, store extent read from shapefile

   if (bOK && pRect != NULL)
   {
	   pRect->left = polyline.m_dBox[0];
	   pRect->right = polyline.m_dBox[2];
	   pRect->top = polyline.m_dBox[1];
	   pRect->bottom = polyline.m_dBox[3];

	   if (bLatLon)
	   {
	       BDProjection()->LatLonToTransMercator(pRect->top, pRect->left, &pRect->left, &pRect->top);
		   BDProjection()->LatLonToTransMercator(pRect->bottom, pRect->right, &pRect->right, &pRect->bottom);
	   }
	   if (pRect->top > pRect->bottom) Swap(pRect->top, pRect->bottom);
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Imports a single point into the array of points from a shapefile
//

BOOL CShapeFile::ImportPoints(CCoordArray* pPoints, FILE* pFile, BOOL bLatLon)
{
   CShapePoint shapepoint;   
   BOOL bOK = TRUE;   
   CCoord coord;

   // Read header

   if (fread(&shapepoint, sizeof(shapepoint), 1, pFile))
   {
      double dX = shapepoint.m_dX;
      double dY = shapepoint.m_dY;

		if (bLatLon)
		{
		  BDProjection()->LatLonToTransMercator(dY, dX, &coord.x, &coord.y);
      } else
      {
         coord.x = (long)(dX + 0.5);
         coord.y = (long)(dY + 0.5);
      }
		pPoints->Add(coord);  
   } else
   {
      bOK = FALSE;
   }   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////


BOOL CShapeFile::ExportShapeFile(CLongLines* pMapLines, LPCSTR sFileName, int nFlags)
{
   BOOL bOK;

   // Convert the maplines into a map object and export it
 
   CMapLayer maplayer;
   
   CMapLayerObj mapobj;
   mapobj.SetMapObject(pMapLines);
   mapobj.SetDataType(BDMAPLINES);
   maplayer.Add(&mapobj);

   bOK = ExportShapeFile(&maplayer, sFileName, nFlags);

   if (bOK)
   {
      AfxMessageBox(BDString(IDS_SHAPEFILEEXPORTED));
   } else
   {
      AfxMessageBox(BDString(IDS_ERROREXPORT));      
   }

   mapobj.SetMapObject(NULL);
   maplayer.CArray <CMapLayerObj*,  CMapLayerObj*>::RemoveAll(); // Prevents deletion of memory

   return bOK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Exports map lines in the format of a shape file
//

BOOL CShapeFile::ExportShapeFilePoly(CMapLayer* pMapLayer, LPCSTR sFileName, int nFlags)
{
   BOOL bOK = TRUE;
   CMainHeader mainheader;   
   CPolyLine polyline;
   DWORD nShape = SHPPolyLine;
   FILE* pFile = NULL;   
  
   // Retrieve query data

   BOOL bLatLon = nFlags & LatLon;
   
   if (nFlags & Polygon) nShape = nShape = SHPPolygon;

   // If polygon, make sure that the last point is closed

   if (nShape == SHPPolygon)
   {
      PolylineToPolygon(pMapLayer);
   };

  // Determine the extend of the points

   polyline = GetBoundingBox(pMapLayer, bLatLon);
   
   memset(&mainheader,0,sizeof(mainheader));      
   mainheader.m_dXMin = polyline.m_dBox[0];
   mainheader.m_dXMax = polyline.m_dBox[2];
   mainheader.m_dYMin = polyline.m_dBox[1];
   mainheader.m_dYMax = polyline.m_dBox[3];   
   
  // Initialise the polyline

   polyline.m_nParts = 0;
   polyline.m_nPoints = 0;      

  // First create the header

   mainheader.m_nFileCode = ReverseBytes(9994);      
   mainheader.m_nVersion = 1000;
   mainheader.m_nShapeType = nShape;   

   // Count the number of points, exclude repeating null values
      
   int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
   {     
      CMapLayerObj* pMapObj = pMapLayer->GetAt(j);
      if (pMapObj->GetDataType() == BDMAPLINES)
      {
         CLongLines* pMapLines = (CLongLines*)pMapObj->GetMapObject(); 

         CPolyLine polylineP = GetNumParts(pMapLines);
         polyline.m_nParts += polylineP.m_nParts;
         polyline.m_nPoints += polylineP.m_nPoints;         
      }
   };

   // Determine the file length
   
   // Support for multipart polylines
      int nFileLength = sizeof(CMainHeader) + (sizeof(CRecordHeader) + sizeof(CPolyLine) + sizeof(int)) * pMapLayer->GetSize() + 
                     sizeof(int) * polyline.m_nParts + sizeof(double)*2 * polyline.m_nPoints;

    mainheader.m_nFileLength = ReverseBytes(nFileLength/2);
     
   // Open the file as binary

   TRY
   {   
      pFile = fopen(sFileName, "wb");
      if (pFile != NULL)
      {
         // Write the file header

         if (fwrite(&mainheader, sizeof(CMainHeader),1,pFile) == 1)
         {         
            bOK = ExportPolyLine(pMapLayer, pFile, bLatLon, nShape);
         } else
         {
            bOK = FALSE;
         }
   }
   } CATCH(CException, ex)
   {
      bOK = FALSE;
   } END_CATCH

   if (pFile != NULL) fclose(pFile);

   // Export the index file
   
   if (bOK)
   {      
      bOK = ExportIndexFile(polyline, pMapLayer, sFileName, mainheader);
   };
   
  // Export DBASE format
 
   if (bOK)
   {  
      if (pMapLayer->GetQuery() != NULL)
      {                
         bOK = ExportDBaseQuery(pMapLayer, sFileName);
      }
      else
      {  
         // Used for exporting from editor (no query)
         bOK = ExportDBase(pMapLayer, sFileName);
      } 
   };

   ASSERT(AfxCheckMemory());

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::PolylineToPolygon(CMapLayer* pMapLayer)
{
   CLongCoord coord1, coord2;

   int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
   {
      CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(j);
      CLongLines* pMapLines = (CLongLines*)pMapLayerObj->GetMapObject();

      // For each map lines, close polygons
      
      int i = 0; for (i = 0; i < pMapLines->GetSize(); i++)
      {
         // Determine the start of a line

         if (!pMapLines->GetAt(i).IsNull() && 
             (i == 0 || (i > 0 && pMapLines->GetAt(i-1).IsNull())))             
         {
            coord1 = pMapLines->GetAt(i);
         }

         // Determine the end of a line

         if (i + 1 < pMapLines->GetSize() && 
             pMapLines->GetAt(i+1).IsNull() &&
             !pMapLines->GetAt(i).IsNull())
         {
            coord2 = pMapLines->GetAt(i);

            // If the two ends are not the same then make them the same

            if (coord1.x != coord2.x && coord1.y != coord2.y)
            {
               pMapLines->InsertAt(i+1, coord1);  
               i++;
            }      
         }         
      }
   }  
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Exports the polyline part of the shape file - retains multipart polylines
//

BOOL CShapeFile::ExportPolyLine(CMapLayer* pMapLayer, FILE* pFile, BOOL bLatLon, DWORD nShape)
{
   BOOL bOK = TRUE;   
   CRecordHeader recordheader;      
   CPolyLine polyline;

   // Determine parts

   int nRecord = 1;   
   int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
   {      
      BOOL bStart = TRUE;

      CMapLayerObj* pMapObj = pMapLayer->GetAt(j);
      if (pMapObj->GetDataType() == BDMAPLINES)
      {
         CLongLines* pMapLines = (CLongLines*)pMapObj->GetMapObject(); 

         // Determine bounding rectangle

         polyline = GetBoundingBoxPoly(pMapLines, bLatLon);            

         // For each polyline determine its parts

         CLongArray anParts;
         anParts.Add(0);

         polyline.m_nPoints = 0;
         for (int k = 0; k < pMapLines->GetSize(); k++)
         {
            CLongCoord coord = pMapLines->GetAt(k);
            if (!coord.IsNull()) polyline.m_nPoints++;
            else if (k + 1 < pMapLines->GetSize()) anParts.Add(polyline.m_nPoints);             
         }
         polyline.m_nParts = anParts.GetSize();         

         // Write record header

         recordheader.m_nRecordNumber = ReverseBytes(nRecord++);
         int nRecordLength = sizeof(int) + sizeof(CPolyLine) + sizeof(int)*polyline.m_nParts + 
                             sizeof(double)*2 * polyline.m_nPoints;
         recordheader.m_nContentLength = ReverseBytes(nRecordLength/2);

         fwritex(&recordheader, sizeof(CRecordHeader),1,pFile);
         
         // Write shape type

         fwritex(&nShape,sizeof(nShape),1,pFile);

         // Write polyline record contents

         fwritex(&polyline, sizeof(CPolyLine),1,pFile);

         // Write the parts data

         fwritex(anParts.GetData(), sizeof(int),anParts.GetSize(),pFile);
                        
         // Write the points data         

         int i = 0; for (i = 0; i < pMapLines->GetSize() && bOK; i++)
         {
            CLongCoord coord = pMapLines->GetAt(i);
            if (!coord.IsNull())
            {
               // Output the data

               double dX = coord.x;
               double dY = coord.y;

               if (bLatLon)
			      {                
				      BDProjection()->TransMercatorToLatLon(dX, dY, &dY, &dX);
               } else
               {

               };

               fwritex(&dX, sizeof(double), 1, pFile);
               fwritex(&dY, sizeof(double), 1, pFile);                  
            }
         }        
      }
   }
   
   return bOK;
}



///////////////////////////////////////////////////////////////////////////////
//
// Export an entire layer to a shape file (as points)
//

BOOL CShapeFile::ExportPoints(CMapLayer* pMapLayer, FILE* pFile, BOOL bLatLon)
{   
   BOOL bOK = TRUE;
   DWORD nShape = SHPPoint;
   CRecordHeader recordheader;      
   CShapePoint point; 

   // Determine parts

   int nRecord = 1;   
   int i = 0; for (i = 0; i < pMapLayer->GetSize() && bOK; i++)
   {
      CCoord* pCoord = (CCoord*)pMapLayer->GetAt(i)->GetMapObject();      

      if (!pCoord->IsNull())
      {

         // Initialise the header

         recordheader.m_nRecordNumber = ReverseBytes(nRecord++);                              
         int nRecordLength = sizeof(CShapePoint) + sizeof(int);
         recordheader.m_nContentLength = ReverseBytes(nRecordLength/2);

         // Write the record header

         fwritex(&recordheader, sizeof(CRecordHeader),1,pFile);

         // Write the shape type

         fwritex(&nShape,sizeof(nShape),1,pFile);
   
         // Write the x,y data

         point.m_dX = pCoord->x;
         point.m_dY = pCoord->y;

         if (bLatLon)
		   {                
			   BDProjection()->TransMercatorToLatLon(point.m_dX, point.m_dY, &point.m_dY, &point.m_dX);
         } else
         {

         };
         fwritex(&point, sizeof(CShapePoint), 1, pFile);                        
      };
   }
   
   return bOK;   
}


///////////////////////////////////////////////////////////////////////////////
//
// Export the index file (points)
//

BOOL CShapeFile::ExportIndexFile(CMapLayer* pMapLayer, LPCSTR sFileName, CMainHeader& mainheader)
{
   CIndexRecord indexrecord;   
   FILE* pFile = NULL;
   BOOL bOK = TRUE;
   
   CString sIndexFile = sFileName;
   sIndexFile = sIndexFile.Left(sIndexFile.ReverseFind('.'));
   sIndexFile += ".shx";      

   TRY
   {
      pFile = fopen(sIndexFile,"wb");
      if (pFile != NULL)
      {
       // Write the header

         int nFileLength = sizeof(CMainHeader)+sizeof(CIndexRecord)*pMapLayer->GetSize();
         mainheader.m_nFileLength = ReverseBytes(nFileLength/2);
         fwritex(&mainheader, sizeof(CMainHeader),1,pFile);
         
       // Determine the indexes
         
         indexrecord.m_nOffset = ReverseBytes((sizeof(CMainHeader)-sizeof(CRecordHeader))/2);
         indexrecord.m_nContentLength = 0;

         int i = 0; for (i = 0; i < pMapLayer->GetSize() && bOK; i++)
         {
            CLongCoord* pCoord = (CLongCoord*)pMapLayer->GetAt(i)->GetMapObject();      
            
            // Write record header

            indexrecord.m_nOffset = ReverseBytes(indexrecord.m_nContentLength)+
                                    ReverseBytes(indexrecord.m_nOffset) + 
                                    sizeof(CRecordHeader)/2;
            indexrecord.m_nOffset = ReverseBytes(indexrecord.m_nOffset);
                                                            
            int nRecordLength = sizeof(CShapePoint) + sizeof(int);
            indexrecord.m_nContentLength = ReverseBytes(nRecordLength/2);

            // Write the record header

            fwritex(&indexrecord, sizeof(CIndexRecord),1,pFile);                     
         }               
      } else
      {
         bOK = FALSE;
      }
   }
   CATCH (CException, ex)
   {
      bOK = FALSE;
   } END_CATCH

   if (pFile != NULL) fclose(pFile);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Export the index file multi-part polygons
//


BOOL CShapeFile::ExportIndexFile(CPolyLine& polyline, CMapLayer* pMapLayer, LPCSTR sFileName, CMainHeader& mainheader)
{
   CIndexRecord indexrecord;   
   FILE* pFile = NULL;
   BOOL bOK = TRUE;
   
   CString sIndexFile = sFileName;
   sIndexFile = sIndexFile.Left(sIndexFile.ReverseFind('.'));
   sIndexFile += ".shx";      

   TRY
   {
      pFile = fopen(sIndexFile,"wb");
      if (pFile != NULL)
      {
       // Write the header

         int nFileLength = sizeof(CMainHeader)+sizeof(CIndexRecord)*pMapLayer->GetSize();
         mainheader.m_nFileLength = ReverseBytes(nFileLength/2);
         fwritex(&mainheader, sizeof(CMainHeader),1,pFile);
         
       // Determine the indexes
         
         indexrecord.m_nOffset = ReverseBytes(sizeof(CMainHeader)/2);         

         int j = 0; for (j = 0; j < pMapLayer->GetSize() && bOK; j++)
         {
            CMapLayerObj* pMapObj = pMapLayer->GetAt(j);
            if (pMapObj->GetDataType() == BDMAPLINES)
            {               
               // For each polyline determine its parts

               CLongLines* pMapLines = (CLongLines*)pMapObj->GetMapObject();          
               polyline.m_nPoints = 0;
               polyline.m_nParts = 1;
               for (int k = 0; k < pMapLines->GetSize(); k++)
               {
                  CLongCoord coord = pMapLines->GetAt(k);
                  if (!coord.IsNull()) polyline.m_nPoints++;
                  else if (k + 1 < pMapLines->GetSize()) polyline.m_nParts++;
               }               
               int nRecordLength = sizeof(int) + sizeof(CPolyLine) + sizeof(int)*polyline.m_nParts + 
                                   sizeof(double)*2 * polyline.m_nPoints;

               // Write index
               indexrecord.m_nContentLength = ReverseBytes(nRecordLength/2);
              fwritex(&indexrecord, sizeof(CIndexRecord),1,pFile);         

               // Update the next index record
               
               int nOffSet = ReverseBytes(indexrecord.m_nOffset)*2 + nRecordLength + 
                  sizeof(CRecordHeader);
               indexrecord.m_nOffset = ReverseBytes(nOffSet/2);                                                                   
            } else
            {
                  bOK = FALSE;
            }
         }
      } else
      {
         bOK = FALSE;
      }
   }
   CATCH (CException, ex)
   {
      bOK = FALSE;
   } END_CATCH

   if (pFile != NULL) fclose(pFile);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::ExportShapeFile(CMapLayer* pMapLayer, LPCSTR sFileName, int nFlags)
{  
   BOOL bOK = TRUE;  

   if (pMapLayer->GetSize() == 0)
   {
      AfxMessageBox(BDString(IDS_NODATALAYER));     
      return -1; // Prevents display of error message
   }


   if (pMapLayer->GetAt(0)->GetDataType() == BDMAPLINES)
   {
      bOK = ExportShapeFilePoly(pMapLayer, sFileName, nFlags);               
   };

   if (pMapLayer->GetAt(0)->GetDataType() == BDCOORD)
   {
      bOK = ExportShapeFilePoint(pMapLayer, sFileName, nFlags);               
   };

   return bOK;
};

///////////////////////////////////////////////////////////////////////////////
//
// This exports an entire layer to a shape file
//

BOOL CShapeFile::ExportShapeFilePoint(CMapLayer* pMapLayer, LPCSTR sFileName, BOOL bLatLon)
{
   BOOL bOK = TRUE;
   CMainHeader mainheader;   
   DWORD nShape = SHPPoint;
   FILE* pFile = NULL;

  // Determine the extent of the points

   CPolyLine polyline = GetBoundingBox(pMapLayer, bLatLon);

   memset(&mainheader,0,sizeof(mainheader));   
   mainheader.m_dXMin = polyline.m_dBox[0];
   mainheader.m_dXMax = polyline.m_dBox[2];
   mainheader.m_dYMin = polyline.m_dBox[1];
   mainheader.m_dYMax = polyline.m_dBox[3];
   
  // Initialise the polyline

   polyline.m_nParts = 0;
   polyline.m_nPoints = 0;      

  // First create the header

   mainheader.m_nFileCode = ReverseBytes(9994);      
   mainheader.m_nVersion = 1000;
   mainheader.m_nShapeType = nShape;   
      
   // Determine the file length

   int nPoints = 0;
   for (int i =0; i < pMapLayer->GetSize(); i++)
   {
      CCoord* pCoord = (CCoord*)pMapLayer->GetAt(i)->GetMapObject();      
      if (!pCoord->IsNull())
      {
         nPoints++;
      }
   }
   
   int nFileLength = sizeof(CMainHeader)+(sizeof(CRecordHeader)+sizeof(CShapePoint) + sizeof(int)*2)*nPoints;                                          
   mainheader.m_nFileLength = ReverseBytes(nFileLength/2);
     
   // Open the file as binary

   TRY
   {   
      pFile = fopen(sFileName, "wb");
      if (bOK && pFile != NULL)
      {
         // Write the file header

         if (fwrite(&mainheader, sizeof(CMainHeader),1,pFile) == 1)
         {         
            bOK = ExportPoints(pMapLayer, pFile, bLatLon);
         } else
         {
            bOK = FALSE;
         }
      } else
      {
         bOK = FALSE;
      }
   } CATCH(CException, ex)
   {
      bOK = FALSE;
   } END_CATCH

   if (pFile != NULL) fclose(pFile);

   // Export the index file
   
   if (bOK)
   {      
      bOK = ExportIndexFile(pMapLayer, sFileName, mainheader);
   };
   
   
  // Export DBASE format
   
   if (bOK)
   {
      if (pMapLayer->GetQuery() != NULL)
      {
         bOK = ExportDBaseQuery(pMapLayer, sFileName);
      } else
      {         
         bOK = ExportDBase(pMapLayer, sFileName);
      }
   };

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieves a bounding box for an entire layer
//

CPolyLine CShapeFile::GetBoundingBox(CMapLayer* pMapLayer, BOOL bLatLon)
{   
   // Initialise polyline

   CPolyLine polyline;

   polyline.m_dBox[0] = DBL_MAX;
   polyline.m_dBox[1] = DBL_MAX;
   polyline.m_dBox[2] = -DBL_MAX;
   polyline.m_dBox[3] = -DBL_MAX;

   int i = 0; for (i = 0; i < pMapLayer->GetSize(); i++)
   {
      // Map layer

      CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(i);
      if (pMapLayerObj->GetDataType() == BDMAPLINES)
      {                  
         CLongLines* pMapLines = (CLongLines*)pMapLayerObj->GetMapObject();
         CPolyLine polylineP = GetBoundingBoxPoly(pMapLines, bLatLon);         

         polyline.m_dBox[0] = min(polyline.m_dBox[0], polylineP.m_dBox[0]);
         polyline.m_dBox[1] = min(polyline.m_dBox[1], polylineP.m_dBox[1]);
         polyline.m_dBox[2] = max(polyline.m_dBox[2], polylineP.m_dBox[2]);
         polyline.m_dBox[3] = max(polyline.m_dBox[3], polylineP.m_dBox[3]);
      }
      
      // Points

      else if (pMapLayerObj->GetDataType() == BDCOORD)
      {
         CCoord coord = *(CCoord*)pMapLayerObj->GetMapObject();
         
         // Convert to lat/lon if necessary         

         double dX, dY;

         if (bLatLon)
         {
            BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dY, &dX);            
         } else
         {
             dX = coord.x;
             dY = coord.y;

         }

         polyline.m_dBox[0] = min(polyline.m_dBox[0], dX);
         polyline.m_dBox[1] = min(polyline.m_dBox[1], dY);
         polyline.m_dBox[2] = max(polyline.m_dBox[2], dX);
         polyline.m_dBox[3] = max(polyline.m_dBox[3], dY);         
      }
   }   

   return polyline;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieves the bounding box for a map line
//

CPolyLine CShapeFile::GetBoundingBoxPoly(CLongLines* pMapLines, BOOL bLatLon, int i1, int i2)
{
   CPolyLine polyline;

   if (i1 == i2) i2 = pMapLines->GetSize();
   
   polyline.m_dBox[0] = DBL_MAX;
   polyline.m_dBox[1] = DBL_MAX;
   polyline.m_dBox[2] = -DBL_MAX;
   polyline.m_dBox[3] = -DBL_MAX;

   for (int i = i1; i < i2; i++)
   {         
      CLongCoord coord = pMapLines->GetAt(i);
      if (!coord.IsNull())
      {         
         polyline.m_dBox[0] = min(polyline.m_dBox[0], coord.x);
         polyline.m_dBox[1] = min(polyline.m_dBox[1], coord.y);
         polyline.m_dBox[2] = max(polyline.m_dBox[2], coord.x);
         polyline.m_dBox[3] = max(polyline.m_dBox[3], coord.y);
      };         
   }

   // Convert to lat/lon if necessary         

   if (bLatLon)
   {
      BDProjection()->TransMercatorToLatLon(polyline.m_dBox[0], polyline.m_dBox[1], 
                            &polyline.m_dBox[1], &polyline.m_dBox[0]);
      BDProjection()->TransMercatorToLatLon(polyline.m_dBox[2], polyline.m_dBox[3], 
                            &polyline.m_dBox[3], &polyline.m_dBox[2]);

      // Swap if min > max

      if (polyline.m_dBox[0] > polyline.m_dBox[2])
      {
         double d = polyline.m_dBox[0];
         polyline.m_dBox[0] = polyline.m_dBox[2];
         polyline.m_dBox[2] = d;
      }
   } else
   {

   }

   return polyline;
}

///////////////////////////////////////////////////////////////////////////////

CPolyLine CShapeFile::GetNumParts(CLongLines* pMapLines)
{
   CPolyLine polyline;

   BOOL bStart = TRUE;
   int i = 0; for (i = 0; i < pMapLines->GetSize(); i++)
   {
      CLongCoord coord = pMapLines->GetAt(i);
      if (coord.IsNull())
      {
         bStart = TRUE;                  
      }
      else
      {
         if (bStart) polyline.m_nParts++;
         polyline.m_nPoints++;      
         bStart = FALSE;
      }
   } 
   return polyline;
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines the text to be exported with the object
//

CString CShapeFile::GetLineText(CMapLayer* pMapLayer, CMapLayerObj* pMapLayerObj)
{
   if (pMapLayer->GetSepColour())
   {
      int i = 0; for (i = 0; i < pMapLayer->GetMapProp().m_aColourFeature.GetSize(); i++)
      {
         CColourFeature &rColourFeature = pMapLayer->GetMapProp().m_aColourFeature.GetAt(i);

         if (pMapLayer->GetSepColour() == CMapLayer::SepColourFeature)
         {           
            if (rColourFeature.m_lFeatureId == pMapLayerObj->GetFeature())
            {
               return rColourFeature.m_sFeature;               
            }
         } else
         {
            if (rColourFeature.m_sAttr == pMapLayerObj->GetText())
            {
               return rColourFeature.m_sFeature;               
            }
         }
      }
      return "";
   } else
   {
      return pMapLayerObj->GetText();
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Export the dbase data for shapefile.  For NRDBPro,  this includes ALL 
// columns from the query
//

BOOL CShapeFile::ExportDBaseQuery(CMapLayer* pMapLayer, LPCSTR sFileName)
{
   BOOL bOK = TRUE;
   CDBFHeader header;
   CDBFDescriptor descriptor;   

   CQuery* pQuery = pMapLayer->GetQuery();

   // Retrieve data

   CDlgProgress dlg;   

   CQueryResult queryresult;
   if (!queryresult.Initialise(pQuery) || 
       queryresult.GetSize() == 0)
   {
      bOK = FALSE;
   }
   
   // Create file name

   CString sIndexFile = sFileName;
   sIndexFile = sIndexFile.Left(sIndexFile.ReverseFind('.'));
   sIndexFile += ".dbf";      

   FILE* pFile = fopen(sIndexFile,"wb");
     
   TRY
   {

      if (bOK && pFile != NULL)
      {

         // Create header
   
         CDateTime date;
         date.AsSystemDate();
         memset(&header, 0, sizeof(header));
         header.m_nValid = 0x03;
         header.m_aDate[0] = date.GetYear()%100+100;
         header.m_aDate[1] = date.GetMonth();
         header.m_aDate[2] = date.GetDay();
         
         header.m_nNumRecords = pMapLayer->GetSize();
         
         // Determine number of record bytes and fields

         CAttrArray& aAttr = *queryresult.GetAt(0);
         int nRecordBytes = 0,
			 nFields = 0,
			 i = 0;

         for (i = 0; i < aAttr.GetSize(); i++)
         {
            switch (aAttr[i]->GetDataType())
            {
			      case BDTEXT : case BDLINK : case BDFEATURE : case BDFTYPE : case BDLONGTEXT :
                 nRecordBytes += DBF_TEXT; nFields++; break;
               case BDBOOLEAN : nRecordBytes += DBF_BOOL; nFields++; break;
               case BDNUMBER : nRecordBytes += DBF_NUMBER; nFields++; break;
               case BDDATE : nRecordBytes += DBF_DATE; nFields++; break;
            }
          
         }      

         // Set number of field descriptor bytes
      
         header.m_nHeaderBytes = sizeof(header) + sizeof(CDBFDescriptor) *
            nFields + sizeof(BYTE);
         header.m_nRecordBytes = nRecordBytes + sizeof(BYTE);   

         header.m_nReserved3[1] = 0;

         fwritex(&header,sizeof(header),1,pFile);

         // Create the record descriptors
      
         for (i = 0; i < aAttr.GetSize(); i++)
         {
           memset(&descriptor,0,sizeof(descriptor));
           strncpy(descriptor.m_sName, aAttr[i]->GetDesc(), 10);

           switch (aAttr[i]->GetDataType())
           {
		       case BDTEXT : case BDLINK : case BDFEATURE : case BDFTYPE : case BDLONGTEXT :
                  descriptor.m_nType = 'C';
                  descriptor.m_nFieldLength = DBF_TEXT;
                  break;
               case BDNUMBER : 
                  descriptor.m_nType = 'N';
                  descriptor.m_nFieldLength = DBF_NUMBER;
                  descriptor.m_nFieldCount = DBF_NDP;
                  break;
               case BDDATE : 
                  descriptor.m_nType = 'D';
                  descriptor.m_nFieldLength = DBF_DATE;
                  break;
               case BDBOOLEAN :
                  descriptor.m_nType = 'L';
                  descriptor.m_nFieldLength = DBF_BOOL;
                  break;
           }
           if (descriptor.m_nType != 0)
           {
              fwritex(&descriptor,sizeof(descriptor),1,pFile);
           }
        }

        BYTE nTerminator = 0x0D;
        fwritex(&nTerminator,sizeof(BYTE),1,pFile);         

        // Write the data

        for (i = 0; i < queryresult.GetSize(); i++)
        {
           CAttrArray &aAttr = *queryresult.GetAt(i);

           if (IsValidMapObject(aAttr, queryresult.GetAttrArray()))
           {                       
              // Write the record separator

              BYTE nRecord = 0x20;
              fwritex(&nRecord,sizeof(BYTE),1,pFile);

              // Write the attribute data

              int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
              {
                 CString s;
                 char record[DBF_TEXT];              
                 memset(&record, 0x20, sizeof(record));

                 CAttribute* pAttr = aAttr.GetAt(j);

                 int nLength = 0;
                 switch(pAttr->GetDataType())
                 {
			        case BDTEXT : case BDLINK : case BDFEATURE : case BDFTYPE : case BDLONGTEXT :
                       s = pAttr->AsString();
                       strncpy(record, s, min(DBF_TEXT, s.GetLength()));
                       nLength = DBF_TEXT;
                       break;
                    case BDNUMBER :
                       if (!pAttr->IsNull())
                       {
                          s.Format("%*lf", DBF_NUMBER, *pAttr->GetDouble());
                       } else
                       {
                          s = "";
                       }
                       strncpy(record, s, DBF_NUMBER);
                       nLength = DBF_NUMBER;
                       break;
                    case BDBOOLEAN : 
                       if (*pAttr->GetBoolean() == TRUE) record[0] = 'Y';
                       else if (*pAttr->GetBoolean() == FALSE) record[0] = 'N';
                       else record[0] = '?';
                       nLength = DBF_BOOL;
                       break;
                    case BDDATE :                    
                       CDateTime date(*pAttr->GetDate(), 0);      
                       CString s1,s2;
                       s.Format("%4d",date.GetYear());                     
                       if (date.GetMonth() != 0) s1.Format("%02d",date.GetMonth());                     
                       if (date.GetDay() != 0)  s2.Format("%02d",date.GetDay());                        
                       s += s1 + s2;
                       strncpy(record, s, min(DBF_DATE, s.GetLength()));
                       nLength = DBF_DATE;
                       break;
                 }
                 if (nLength != 0) fwritex(&record, nLength, 1, pFile);
              }                                                          
           }
        }     
        // Write the end of record

        BYTE nEnd = 0x1a;
        fwritex(&nEnd, sizeof(nEnd), 1, pFile);                    
      } else
      {
         bOK = FALSE;
      }

   } CATCH(CException, ex)
   {      
      bOK = FALSE;
   } END_CATCH

   if (pFile != NULL) fclose(pFile);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Export the dbase data for shapefile
//

BOOL CShapeFile::ExportDBase(CMapLayer* pMapLayer, LPCSTR sFileName)
{
   BOOL bOK = TRUE;
   CDBFHeader header;
   CDBFDescriptor aDescriptor[2];
   CDBFRecordPoint record;

   // Create file name

   CString sIndexFile = sFileName;
   sIndexFile = sIndexFile.Left(sIndexFile.ReverseFind('.'));
   sIndexFile += ".dbf";      

   // Create header
   
   memset(&header, 0, sizeof(header));
  
   header.m_nValid = 0x03;

   // Set update date
   CDateTime date;
   date.AsSystemDate();
   header.m_aDate[0] = date.GetYear()%100+100;
   header.m_aDate[1] = date.GetMonth();
   header.m_aDate[2] = date.GetDay();   

   header.m_nNumRecords = pMapLayer->GetSize();
   header.m_nHeaderBytes = sizeof(header) + sizeof(aDescriptor)+sizeof(BYTE);
   header.m_nRecordBytes = sizeof(CDBFRecordPoint)+1;   
   header.m_nReserved3[1] = 0;

   // Create the record descriptors

   memset(aDescriptor,0,sizeof(aDescriptor));
   memcpy(aDescriptor[0].m_sName,"ID",2);
   aDescriptor[0].m_nType = 'N';
   aDescriptor[0].m_nFieldLength = 0x08;
   
   memcpy(aDescriptor[1].m_sName,"NAME",4);
   aDescriptor[1].m_nType = 'C';
   aDescriptor[1].m_nFieldLength = 0x20;

   BYTE nTerminator = 0x0D;
   BYTE nRecord = 0x20;
   BYTE nEnd = 0x1a;

   // Create the data field

   memset(&record,0x20,sizeof(CDBFRecordPoint));
   
   FILE* pFile = fopen(sIndexFile,"wb");
   if (pFile != NULL)
   {
      // Write header

      if (fwrite(&header,sizeof(header),1,pFile) == 1)
      {
         // Write record descriptors

         int i = 0; for (i = 0; i < sizeof(aDescriptor)/sizeof(CDBFDescriptor) && bOK; i++)
         {
            if (fwrite(&aDescriptor[i],sizeof(CDBFDescriptor),1,pFile) != 1)
            {
               bOK = FALSE;
            }
         }

         // Write field terminator

         if (bOK && fwrite(&nTerminator,sizeof(BYTE),1,pFile) != 1)
         {
            bOK = FALSE;
         }

         // Write the data

         int nLabel = 1;

         for (i = 0 ; i < pMapLayer->GetSize() && bOK; i++)
         {            
             CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(i);
             CString s = GetLineText(pMapLayer, pMapLayerObj);             

             if (pMapLayerObj->GetDataType() == BDCOORD)
             {
                CLongCoord* pCoord = (CLongCoord*)pMapLayerObj->GetMapObject();
                             
                memset(&record,0x20,sizeof(CDBFRecordPoint));
                sprintf(record.m_sID,"%8i", nLabel++);                   
                memcpy(record.m_sName,s,min(sizeof(record.m_sName), s.GetLength()));

                if (fwrite(&nRecord,sizeof(BYTE),1,pFile) != 1 ||
                    fwrite(&record,sizeof(CDBFRecordPoint),1,pFile) != 1)
                {
                   bOK = FALSE;
                }             
                
             } 
             else if (pMapLayerObj->GetDataType() == BDMAPLINES)
             {                                
                CLongLines* pMapLines = (CLongLines*)pMapLayerObj->GetMapObject();                                

     // For NRDBPro only one record per multipart polyline

                {                     
                   memset(&record,0x20,sizeof(CDBFRecordPoint));
                   sprintf(record.m_sID,"%8i", nLabel++);                   
                   memcpy(record.m_sName,s,min(sizeof(record.m_sName), s.GetLength()));

                   if (fwrite(&nRecord,sizeof(BYTE),1,pFile) != 1 ||
                       fwrite(&record,sizeof(CDBFRecordPoint),1,pFile) != 1)
                   {
                      bOK = FALSE;
                   }                 
                };              
             }
         };

         // Write the end of record

         if (bOK && fwrite(&nEnd, sizeof(nEnd), 1, pFile) != 1)            
         {
            bOK = FALSE;
         }
      } else
      {
         bOK = FALSE;
      }
      fclose(pFile);
   } else
   {
      bOK = FALSE;
   }
   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::IsValidMapObject(CAttrArray& aAttr, CArrayAttrSel& aAttrSel)
{
   BOOL bFound = FALSE;

// Determine the map objects to display

   int i = 0; for (i = 0; i < aAttrSel.GetSize() && !bFound; i++)
   {
      int j = 0; for (j = 0; j < aAttr.GetSize() && !bFound; j++)
      {
         if (aAttrSel[i].m_lAttr == aAttr.GetAt(j)->GetAttrId())      
         {
            CAttribute* pAttr = aAttr[j];
            if (pAttr->GetFTypeId() == aAttrSel[i].m_lFType)
            {               
               if (pAttr->GetDataType() == BDMAPLINES)
               {                        
                  CLongLines* pMapLines = new CLongLines(*pAttr->GetLongBinary());
                                
                  if (pMapLines != NULL)
                  {                     
                     bFound = TRUE;
                     break;
                  }                  
               }

               // Add coordinates to the map

               else if (pAttr->GetDataType() == BDCOORD)
               {	              
                  CCoord* pCoord = pAttr->GetCoord();
                  if (pCoord != NULL && !pCoord->IsNull())
                  {                     
                     bFound = TRUE;                   
                     break;
                  }                  
               }               
            };

         };
      };     
   };   
   return bFound;
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts big endian to little endian
//

int CShapeFile::ReverseBytes(int n)
{
   union
   {
      BYTE a[4];
      int n;
   } u,v;

   u.n = n;
      
   v.a[0] = u.a[3];
   v.a[1] = u.a[2];
   v.a[2] = u.a[1];
   v.a[3] = u.a[0];   

   return v.n;
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts a shapefile from one coordinate system to another
//


void CShapeFile::Convert()
{
   BOOL bOK = TRUE;
   m_bLatLon = FALSE;

   if (!BDProjection()->IsDefaultProjection())
   {
      if (!BDProjection()->InitialiseProjection()) return;
   }

   // First select and open the shapefile to determine its coordinate system

   CFileDialog dlg(TRUE, "shp", NULL, OFN_FILEMUSTEXIST, BDString(IDS_SHAPEFILE) + "|*.shp||");

   if (dlg.DoModal() == IDOK)
   {
      m_sFileIn = dlg.GetPathName();
      m_pFileIn = fopen(m_sFileIn, "rb");

      // Read the header

      if (fread(&m_mainheader, sizeof(CMainHeader),1,m_pFileIn) == 1)
      {        
         if (ReverseBytes(m_mainheader.m_nFileCode) != 9994)
         {
            bOK = FALSE;
            AfxMessageBox(BDString(IDS_INVALIDSHAPEFILE));
         }
         if (m_mainheader.m_nShapeType != SHPPoint && 
             m_mainheader.m_nShapeType != SHPPolyLine && 
             m_mainheader.m_nShapeType != SHPPolygon)
         { 
            AfxMessageBox(BDString(IDS_INVALIDSHAPEFILE));
            bOK = FALSE;
         };

         // Determine if lat/long or coordinates

         if (bOK)
         {
            if (m_mainheader.m_dXMin >= -180 && m_mainheader.m_dXMax <= 180 &&
                m_mainheader.m_dYMax <= 90 && m_mainheader.m_dYMin >= -90)
            {
               m_bLatLon = TRUE;
            }

            // Request user if conversion is correct

            CString sMsg;
            if (m_bLatLon)
            {
               sMsg = BDString(IDS_CONVERTLATLON) + " " + BDProjection()->GetProjectionName() + "?";
            } else
            {
               sMsg = BDString(IDS_CONVERTTOLATLON) + " " + BDProjection()->GetProjectionName() + "?";
            }
            if (AfxMessageBox(sMsg, MB_YESNO) != IDYES)
            {
               bOK = FALSE;
            }
         };

         // Determine export file

         CFileDialog dlg(FALSE, "shp", NULL, 0, BDString(IDS_SHAPEFILE) + "|*.shp||");
         if (dlg.DoModal() == IDOK)
         {
            m_sFileOut = dlg.GetPathName();
            if (m_sFileIn != m_sFileOut)
            {
               AfxGetApp()->BeginWaitCursor();
               if (ConvertShapeFile())
               {                
               } else
               {
                  bOK = FALSE;
                  AfxMessageBox(BDString(IDS_ERRORCONVERT));
               }
               AfxGetApp()->EndWaitCursor();
            }             
            else
            {
               AfxMessageBox(BDString(IDS_SAMEFILE));
               bOK = FALSE;
            } 
         } else
         {
            bOK = FALSE;
         }
      };   

      if (m_pFileIn != NULL) fclose (m_pFileIn);

      // Convert index and dbase files

      if (bOK)
      {
         AfxGetApp()->BeginWaitCursor();
         if (ConvertIndex())
         {
            if (!ConvertDBaseFile())
            {
               AfxMessageBox(BDString(IDS_ERRORDBASE));
               bOK = FALSE;
            }
         } else
         {
            AfxMessageBox(BDString(IDS_ERRORINDEX));
            bOK = FALSE;
         }
         AfxGetApp()->EndWaitCursor();
      }    

      if (bOK)
      {
         AfxMessageBox(BDString(IDS_SHAPEFILECONVERTED));
      }
   };   
}

///////////////////////////////////////////////////////////////////////////////
//
// Exports the shapefile
//

BOOL CShapeFile::ConvertShapeFile()
{
   BOOL bOK = TRUE;

    m_pFileOut = fopen(m_sFileOut, "wb");

    if (m_pFileOut != NULL)
    {    
       // Convert the bounding box

       if (m_bLatLon)
       {
          BDProjection()->LatLonToTransMercator(m_mainheader.m_dYMin, m_mainheader.m_dXMin, 
                                &m_mainheader.m_dXMin, &m_mainheader.m_dYMin);
          BDProjection()->LatLonToTransMercator(m_mainheader.m_dYMax, m_mainheader.m_dXMax, 
                                &m_mainheader.m_dXMax, &m_mainheader.m_dYMax);              
       } else
       {

          BDProjection()->TransMercatorToLatLon(m_mainheader.m_dXMin, m_mainheader.m_dYMin, 
                                &m_mainheader.m_dYMin, &m_mainheader.m_dXMin);
          BDProjection()->TransMercatorToLatLon(m_mainheader.m_dXMax, m_mainheader.m_dYMax, 
                                &m_mainheader.m_dYMax, &m_mainheader.m_dXMax);              
       }
       if (m_mainheader.m_dXMin > m_mainheader.m_dXMax) 
       {
          Swap(m_mainheader.m_dXMin, m_mainheader.m_dXMax);
       };
       if (m_mainheader.m_dYMin > m_mainheader.m_dYMax) 
       {
          Swap(m_mainheader.m_dYMin, m_mainheader.m_dYMax);
       };

       // Save the header

       if (fwrite(&m_mainheader, sizeof(CMainHeader),1,m_pFileOut) == 1)
       {
          while (bOK && fread(&m_recordheader, sizeof(CRecordHeader),1,m_pFileIn) == 1)
          {
            bOK &= fwrite(&m_recordheader, sizeof(CRecordHeader), 1, m_pFileOut) == 1;

         // Read the shape type
            if (fread(&m_nShape,sizeof(m_nShape),1,m_pFileIn) == 1)
            {
               bOK &= fwrite(&m_nShape, sizeof(m_nShape), 1, m_pFileOut) == 1;

               // Import only polylines

               if (m_nShape == SHPPolyLine || m_nShape == SHPPolygon)
               {               
                  bOK = ConvertPolyLine();
               } 

               else if (m_nShape == SHPPoint)
               {
                  bOK = ConvertPoints();
               }
               // Skip the record

               else
               {                  
                  AfxMessageBox(BDString(IDS_INVALIDSHAPEFILE));
                  bOK = FALSE;                  
               }
            } else
            {
               bOK = FALSE;
            }
         }
       } else
       {
          bOK = FALSE;
       }
   } else
   {
      bOK = FALSE;
   }    

   if (m_pFileOut != NULL) fclose(m_pFileOut);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::ConvertPolyLine()
{
   BOOL bOK = TRUE;
   DWORD dwParts;
   double adPoints[2];

   CPolyLine polyline;

   if (fread(&polyline, sizeof(CPolyLine), 1, m_pFileIn))
   {
      // Convert extremese

       if (m_bLatLon)
       {
          BDProjection()->LatLonToTransMercator(polyline.m_dBox[1], polyline.m_dBox[0], 
                                &polyline.m_dBox[0], &polyline.m_dBox[1]);
          BDProjection()->LatLonToTransMercator(polyline.m_dBox[3], polyline.m_dBox[2], 
                                &polyline.m_dBox[2], &polyline.m_dBox[3]);

       } else
       {
          BDProjection()->TransMercatorToLatLon(polyline.m_dBox[0], polyline.m_dBox[1], 
                                &polyline.m_dBox[1], &polyline.m_dBox[0]);
          BDProjection()->TransMercatorToLatLon(polyline.m_dBox[2], polyline.m_dBox[3], 
                                &polyline.m_dBox[3], &polyline.m_dBox[2]);
       }
       if (polyline.m_dBox[0] > polyline.m_dBox[2]) 
       {
          Swap(polyline.m_dBox[0], polyline.m_dBox[2]);
       };
       if (polyline.m_dBox[1] > polyline.m_dBox[3]) 
       {
          Swap(polyline.m_dBox[1], polyline.m_dBox[3]);
       };

       // Save
 
       bOK = fwrite(&polyline, sizeof(CPolyLine), 1, m_pFileOut) == 1;
             
      // Read and write parts data
	   DWORD i = 0;
      for (i = 0; bOK && i < polyline.m_nParts; i++)
      {
         //+bOK &= fread(&dwParts, sizeof(DWORD), 1, m_pFileIn) == 1;
         bOK &= fread(&dwParts, sizeof(DWORD), 1, m_pFileIn) == 1;	//temporary... don't know what to do
         bOK &= fwrite(&dwParts, sizeof(DWORD), 1, m_pFileOut) == 1;
      }
      for (i = 0; bOK && i < polyline.m_nPoints; i++)
      {
         bOK &= fread(adPoints, sizeof(adPoints), 1, m_pFileIn) == 1;

         // Convert

         if (m_bLatLon)
         {
            BDProjection()->LatLonToTransMercator(adPoints[1], adPoints[0], &adPoints[0], &adPoints[1]);

         } else
         {
            BDProjection()->TransMercatorToLatLon(adPoints[0], adPoints[1], &adPoints[1], &adPoints[0]);
         }
         
         bOK &= fwrite(adPoints, sizeof(adPoints), 1, m_pFileOut) == 1;
      }
      
   } else
   {
      bOK = FALSE;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::ConvertPoints()
{
   CShapePoint shapepoint;

   BOOL bOK = TRUE;
   if (fread(&shapepoint, sizeof(shapepoint), 1, m_pFileIn))
   {      
		if (m_bLatLon)
		{
		  BDProjection()->LatLonToTransMercator(shapepoint.m_dY, shapepoint.m_dX, 
                              &shapepoint.m_dX, &shapepoint.m_dY);

      } else
      {

        BDProjection()->TransMercatorToLatLon(shapepoint.m_dX, shapepoint.m_dY, 
                              &shapepoint.m_dY, &shapepoint.m_dX);        
      }
      bOK = fwrite(&shapepoint, sizeof(shapepoint), 1, m_pFileOut) == 1;
   } else
   {
      bOK = FALSE;
   }
   return bOK;
		
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::ConvertIndex()
{
   BOOL bOK = TRUE;
   //char ch;

   CMainHeader mainheader;

   CString sIndexIn = m_sFileIn.Left(m_sFileIn.ReverseFind('.')) + ".shx";
   CString sIndexOut = m_sFileOut.Left(m_sFileOut.ReverseFind('.')) + ".shx";

   m_pFileIn = fopen(sIndexIn, "rb");
   m_pFileOut = fopen(sIndexOut, "wb");
   if (m_pFileIn != NULL && m_pFileOut != NULL)
   {
      // Read the header and write the converted one 

       bOK &= fread(&mainheader, sizeof(CMainHeader),1,m_pFileIn) == 1;

       m_mainheader.m_nFileLength = mainheader.m_nFileLength;
       bOK &= fwrite(&m_mainheader, sizeof(CMainHeader),1,m_pFileOut) == 1;

      // Now copy the rest of the file

       CIndexRecord indexrecord;   
       while (bOK && fread(&indexrecord, sizeof(CIndexRecord),1,m_pFileIn) == 1)
       {
          bOK &= fwrite(&indexrecord, sizeof(CIndexRecord),1,m_pFileOut) == 1;
       };       
   } else
   {
      bOK = FALSE;
   }

   if (m_pFileIn != NULL) fclose (m_pFileIn);
   if (m_pFileOut != NULL) fclose(m_pFileOut);    

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CShapeFile::ConvertDBaseFile()
{
   // Copy the dBase file

   BOOL bOK = TRUE;
   char ch;   

   CString sIndexIn = m_sFileIn.Left(m_sFileIn.ReverseFind('.')) + ".dbf";
   CString sIndexOut = m_sFileOut.Left(m_sFileOut.ReverseFind('.')) + ".dbf";

   m_pFileIn = fopen(sIndexIn, "rb");
   m_pFileOut = fopen(sIndexOut, "wb");
   if (m_pFileIn != NULL && m_pFileOut != NULL)
   {   
      // Now copy the rest of the file

       while (bOK && (ch = getc(m_pFileIn)) != EOF)
       {
          fputc(ch, m_pFileOut);
       }
   } else
   {
      bOK = FALSE;
   }

   if (m_pFileIn != NULL) fclose (m_pFileIn);
   if (m_pFileOut != NULL) fclose(m_pFileOut);    

   return bOK;
}
