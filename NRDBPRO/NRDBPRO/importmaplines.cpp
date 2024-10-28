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
#include <string.h>
#include "nrdb.h"
#include "projctns.h"
#include "importmaplines.h"
#include "shapefile.h"
#include "spatial.h"

///////////////////////////////////////////////////////////////////////////////
//

BOOL ImportMapLines(CLongLines& maplines, CString sFileName)
{
   FILE* pFile = NULL;   
   CLongCoord coord;   
   char sTemp[64];
   BOOL bOK = TRUE;
   int iLine = 0;
    
   // Open the file that data is to be imported from  
   // Nb. To distinguish between shapefile with x,y or lat/long coordinates a
   // different extension is used (although both must be present).

   if (sFileName == "")
   {
	   CFileDialog dlg(TRUE, "shp", NULL, 0, BDString(IDS_SHAPEFILE) + "|*.shp|" + 
					   BDString(IDS_POLYLINESFILE) + " (*.txt)|*.txt;*.ll|");
	   if (dlg.DoModal() == IDOK)
	   {
		   sFileName = dlg.GetPathName();
	   };
   };
   if (sFileName != "")
   {
	   AfxGetApp()->BeginWaitCursor();
      
      if (sFileName.Right(4).CompareNoCase(".shp") == 0)
      {
		// If .shx file then include convert from lat/long, 
		// change extension back to shp

        CString sPath = sFileName;

         // Import

         CShapeFile shapefile;
         if (!shapefile.ImportShapeFile(&maplines, sPath))
         {
            AfxMessageBox(BDString(IDS_ERRORSHAPEFILE));
         }
      } 
      // Map lines

      else
      {
         pFile = fopen(sFileName, "r");         
         if (pFile != NULL)
         {  
            // Check for leading number            
            
            do
            {
               BOOL bHeader = TRUE;
               sTemp[0] = '\0';

               fgets(sTemp, 64, pFile);
               iLine++;
               if (sTemp[0] == '\0') break;               

               // Check if leading header

               int i = 0; for (i = 0; bOK && sTemp[i] != '\0' && i < sizeof(sTemp) && bHeader; i++)
               {
                  if ((sTemp[i] < '0' || sTemp[i] > '9') && !isspace(sTemp[i])) bHeader = FALSE;
               }

			   // Check for map layer
               if (sTemp[0] == '[' && sTemp[strlen(sTemp)-2] == ']')
			   {
				   bHeader = TRUE;
			   }

               if (bOK && bHeader) 
               {
                  bOK = fgets(sTemp, 64, pFile) != NULL;
                  iLine++;
               }

               // Read lat/lon or coordinates

               while (bOK && strnicmp(sTemp,"end",3) != 0)
               {
                  if (bOK)
                  {
                     // Determine if bearing

                     if (BDProjection()->StringAsBearing(sTemp, coord) && 
                         maplines.GetSize() > 0)
                     {
                        CLongCoord coordP = maplines.GetAt(maplines.GetSize()-1);
                        if (!coordP.IsNull())
                        {
                           coord.x = coordP.x + coord.x;
                           coord.y = coordP.y + coord.y;

                        } else
                        {
                           bOK = FALSE;
                        }
                     }
                     else
                     {
                        // Convert string to coord

                        int nRet = BDProjection()->StringAsCoord(sTemp, &coord);
                        if (nRet == PR_COORD)
                        {                           
                        } 
                        else if (!nRet) 
                        {
                           bOK = FALSE;
                        }
                     };

                     if (bOK)
                     {
                        // store

                        maplines.Add(coord);

                        // whilst more

                        bOK = fgets(sTemp, 64, pFile) != NULL;
                        iLine++;
                     };
                  };

               }
               coord.SetNull();
               maplines.Add(coord);          

            } while (bOK && strnicmp(sTemp,"end",3) == 0);
            
         }
         else
         {
		     bOK = FALSE;
         }      

         // Display error message

         if (!bOK)
         {
            CString s;
            s.Format(BDString(IDS_ERRORIMPORT) + ": %i", iLine);
            AfxMessageBox(s);
         }
      }      
	  AfxGetApp()->EndWaitCursor();
   } else
   {
      bOK = FALSE;
   }

   if (pFile != NULL) fclose(pFile);   

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL ExportMapLines(CLongLines* pMapLines)
{
   FILE* pFile = NULL;
   BOOL bOK = TRUE;
   CCoord coord;   
   double dLat, dLong;
   CString sLat, sLong;

  // Check to see if maplines are protected
   
   if (pMapLines->IsProtected())
   {
      AfxMessageBox(pMapLines->GetProtected());      
      return TRUE;
   }   

   // Display file export dialog
   CString sFilter = BDString(IDS_SHAPEFILE) + " (" + BDString(IDS_LATLON) + ")|*.shx|";
   if (!BDProjection()->IsLatLon())
   {
       sFilter += BDString(IDS_SHAPEFILE) + " (" + BDString(IDS_XY) + ")|*.shp|";
   }
   sFilter += BDString(IDS_POLYLINESFILE) + " (*.txt)|*.txt|";
   if (!BDProjection()->IsLatLon()) sFilter += BDString(IDS_LATLONFILE) + " (*.txt)|*.ll||";
   
   CFileDialog dlg(FALSE, "shp", NULL, 0, sFilter);
   
   if (dlg.DoModal() == IDOK)
   {     
      AfxGetApp()->BeginWaitCursor();      

	   if (dlg.GetFileExt().CompareNoCase("shp") == 0 || 
          dlg.GetFileExt().CompareNoCase("shx") == 0)
      {         
         // If .shx file then include convert from lat/long, 
		   // change extension back to shp

		   BOOL bLatLon = dlg.GetFileExt().CompareNoCase("shx") == 0;
         CString sPath = dlg.GetPathName();
		   sPath.SetAt(sPath.GetLength()-1,'p');

         int nPolygon = CSpatial::IsPolygon(pMapLines) ? CShapeFile::Polygon : 0;

         CShapeFile shapefile;
         bOK = shapefile.ExportShapeFile(pMapLines, sPath, bLatLon + nPolygon);
      }
      else
      {
         // Change extension to .txt
         CString sPath = dlg.GetPathName();
         sPath = sPath.Mid(0, sPath.Find('.')) + ".txt";

         pFile = fopen(sPath, "w");

         if (pFile != NULL)
         {
            if (BDProjection()->IsLatLon())
            {               
               int i = 0; for (i = 0; i < pMapLines->GetSize() && bOK; i++)
               {    
	               CLongCoord coord = pMapLines->GetAt(i);
	               if (!coord.IsNull())
	               {                                    	                 
	                 BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dLat, &dLong);   
                    BDProjection()->LatLongAsString(dLat, sLat, CProjection::latitude);
                    BDProjection()->LatLongAsString(dLong, sLong, CProjection::longitude);
	                 bOK &= fprintf(pFile, "%s,%s\n", (LPCSTR)sLat, (LPCSTR)sLong) > 0;
	               } else
	               {
                     if (i > 0 && !pMapLines->GetAt(i-1).IsNull())
                     {
	                     bOK &= fprintf(pFile,"end\n") > 0;
                     };
	               };
               };             

            // Otherwise export as a text file

            } else 
            {                 
               int i = 0; for (i = 0; i < pMapLines->GetSize() && bOK; i++)
               {  

	               CLongCoord coord = pMapLines->GetAt(i);
	               while (!coord.IsNull() && bOK && i < pMapLines->GetSize())
	               {                         
                    
	                 bOK &= fprintf(pFile,"%li,%li\n",coord.x, coord.y) > 0;
	                 i++;
                    if (i < pMapLines->GetSize())
                    {
	                    coord = pMapLines->GetAt(i);
                    };
	               } 

                  if (i > 0 && !pMapLines->GetAt(i-1).IsNull())
                  {
	                  bOK &= fprintf(pFile,"end\n") > 0;
                  };
               };                        
            }
         } else
         {
            bOK = FALSE;
         }
         AfxGetApp()->EndWaitCursor();
      };
   };

   if (pFile != NULL) fclose(pFile);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL ExportMapLayer(CMapLayer* pMapLayer, LPCSTR sPath, BOOL bTextLatLon)
{
   BOOL bOK = TRUE;
   double dLat, dLng;
   CString sLat, sLng;

   FILE *pFile = fopen(sPath, "w");
   if (pFile != NULL)
   {
      int i = 0; for (i = 0; i < pMapLayer->GetSize(); i++)
      {
         CMapLayerObj *pMapObj = pMapLayer->GetAt(i);
         
         // Display the object name

         if (CString(pMapObj->GetText()) != "")
         {
            fprintf(pFile, "[%s]\n", (LPCSTR)pMapObj->GetText());
         } else
         {
             fprintf(pFile, "[%i]\n", i);
         }

         // Display the coordinates

         if (pMapObj->GetDataType() == BDMAPLINES)
         {
            CLongLines* pLongLines = (CLongLines*)pMapObj->GetMapObject();
            int j = 0; for (j = 0; j < pLongLines->GetSize(); j++)
            {
                CLongCoord coord = pLongLines->GetAt(j);

                if (!coord.IsNull())
                {
                   if (bTextLatLon)
                   {
                       BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dLat, &dLng);   
                       BDProjection()->LatLongAsString(dLat, sLat, CProjection::latitude);
                       BDProjection()->LatLongAsString(dLng, sLng, CProjection::longitude);                   

                       fprintf(pFile, "%s,%s\n", (LPCSTR)sLat, (LPCSTR)sLng);
                   } else
                   {
                       fprintf(pFile, "%i,%i\n", coord.x, coord.y);
                   }
                } else
                {
                   fprintf(pFile, "end\n");
                }
            }

            // Write the end

            //fprintf(pFile, "end\n");
         } 
         else if (pMapObj->GetDataType() == BDCOORD)
         {
            CCoord* pCoord = (CCoord*)pMapObj->GetMapObject();
            if (bTextLatLon)
            {
                BDProjection()->TransMercatorToLatLon(pCoord->x, pCoord->y, &dLat, &dLng);   
                BDProjection()->LatLongAsString(dLat, sLat, CProjection::latitude);
                BDProjection()->LatLongAsString(dLng, sLng, CProjection::longitude);                   

                fprintf(pFile, "%s,%s\n", (LPCSTR)sLat, (LPCSTR)sLng);
            } else
            {
                fprintf(pFile, "%.0lf,%.0lf\n", pCoord->x, pCoord->y);
            }
         }
      }

      fclose(pFile);
   } else
   {
      bOK = FALSE;
   }

   return bOK;
}