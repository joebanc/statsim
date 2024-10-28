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
#include <io.h>

#include "nrdb.h"
#include "DlgEditAttributes.h"
#include "importmaplines.h"
#include "dlgftypeattr.h"
#include "projctns.h"
#include "bdattribute.h"
#include "dlgeditfeature.h"
#include "fileobj.h"

#ifdef _DEBUG
#include "spatial.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


#define COL_FEATURE 1
#define COL_FEATUREID 2
#define COL_DATE 3
#define ROW_HEADER 0

#define BD_GRAY RGB(128,128,128)
#define BD_WHITE RGB(255,255,255)
#define BD_BLUE RGB(0,0,200)

#define WM_CHANGECELL WM_USER+100

/////////////////////////////////////////////////////////////////////////////
// CDlgEditAttributes dialog

CDlgEditAttributes::CDlgEditAttributes(long lFeature, long lFType, LPCSTR sFeature, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditAttributes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditAttributes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_lFType = lFType;
   m_alFeatures.Add(lFeature);   
   m_sFeature = sFeature;   
   m_sFeature.TrimRight();
   m_pMapLayerObj = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
// Multi-edit constructor
//

CDlgEditAttributes::CDlgEditAttributes(CLongArray& alFeature, long lFType, CWnd* pParent) :
         CDialog(IDD, pParent)
{
   m_alFeatures.Copy(alFeature);
   m_lFType = lFType;
   m_pMapLayerObj = NULL;
}

///////////////////////////////////////////////////////////////////////////////
         
CDlgEditAttributes::CDlgEditAttributes(long lFType, long lFeature, CMapLayerObj* pMapLayerObj, CWnd* pParent) : 
   CDialog(IDD, pParent)
 
{
   m_lFType = lFType;
   m_alFeatures.Add(lFeature);
   m_pMapLayerObj = pMapLayerObj;
}

///////////////////////////////////////////////////////////////////////////////

CDlgEditAttributes::~CDlgEditAttributes()
{
   BDSetProgressBar(NULL);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditAttributes)	
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);   
	//}}AFX_DATA_MAP
}

BEGIN_EVENTSINK_MAP(CDlgEditAttributes, CDialog)
    //{{AFX_EVENTSINK_MAP(CDlgEditAttributes)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BEGIN_MESSAGE_MAP(CDlgEditAttributes, CDialog)
	//{{AFX_MSG_MAP(CDlgEditAttributes)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)		
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_MESSAGE(SSM_LEAVECELL, OnLeaveCell)   
   ON_MESSAGE(WM_CHANGECELL, OnChangeCell)   
   ON_MESSAGE(SSM_DATACHANGE, OnDataChange)
   ON_MESSAGE(SSM_COMBOSELCHANGE, OnComboSelChange)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
   ON_MESSAGE(BDRESETPROGRESS, OnResetProgress)
   ON_MESSAGE(BDUPDATEPROGESS, OnUpdateProgress)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditAttributes message handlers

BOOL CDlgEditAttributes::OnInitDialog() 
{
	CDialog::OnInitDialog();   
   
   BDSetProgressBar(&m_ctlProgress);
   BDSetActiveWnd(GetSafeHwnd());

   CFeatureType ftype;
   ftype.m_lId = m_lFType;
   BDFeatureType(BDHandle(), &ftype, BDSELECT);
   SetWindowText(BDString(IDS_EDIT) + " " + ftype.m_sDesc);
	      
   // Initialise the grid properties

   CreateGrid();
   
   // Initialise the columns

   InitialiseCols();

   // Retrieve all attribute values for the feature
   
   int iRow = ROW_HEADER+1;   
   int i = 0; for (i = 0; i < m_alFeatures.GetSize(); i++)
   {      
      LoadData(m_alFeatures[i], iRow);
   }

   // If initialising a map attribute

   if (m_pMapLayerObj != NULL)
   {
      InitMapObject();
   }

   // Display type of selected cell

   OnChangeCell(0,0);

   // Display insert button for professional version

   GetDlgItem(IDC_INSERT)->ShowWindow(SW_SHOW);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgEditAttributes::GetRowDateTime(long lRow, CDateTime& rDateTime)
{
   BOOL bOK = TRUE;
   CString sDate;
   
   sDate = m_Grid.GetValue(COL_DATE, lRow);   

   return rDateTime.StringAsDateTime(sDate, "");   
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::OnImport() 
{
   CDateTime datetime;
   long lRow, lCol;
   long lDataType, lData;
   CLongLines* pMapLines = NULL;

   m_Grid.GetActiveCell(&lCol, &lRow);
       
   // Import the data

   m_Grid.GetColUserData(lCol, &lDataType);
   if (lDataType == BDMAPLINES)
   {
	   if (GetRowDateTime(lRow, datetime))
	   {
         m_Grid.GetInteger(lCol, lRow, &lData);
         CLongLines* pMapLinesOld = (CLongLines*)(LONG)lData;

         pMapLines = new CLongLines;
         if (pMapLines != NULL)
         {
            if (ImportMapLines(*pMapLines))
            {
#ifdef _DEBUG
/*               CLongBinary longbinary;
               pMapLines->GetLongBinary(longbinary);		
               CLongLines *pMapLines2 = new CLongLines(longbinary);                           
               int i = 0; for (i = 0; i < pMapLines->GetSize();i++)
               {
                  if (!(pMapLines->GetAt(i).x == pMapLines2->GetAt(i).x && pMapLines->GetAt(i).y == pMapLines2->GetAt(i).y))
                  {
                     ASSERT(FALSE);
                     break;
                  }
               }*/
#endif
               m_Grid.SetInteger(lCol, lRow, (LONG)(LPCSTR)pMapLines);                        
               if (pMapLinesOld != NULL) delete pMapLinesOld;
            } else
            {               
               delete pMapLines;
            }                        
            m_Grid.GetInteger(lCol, lRow, &lData);
            if (lData != NULL) m_Grid.SetColor(lCol, lRow, BD_BLUE, BD_BLUE);
            else m_Grid.SetColor(lCol, lRow, BD_WHITE, BD_WHITE);
            
         };            
	   };         
   }

   // Image
   
   // Support for JPG and PNG Images (georeferenced)

   else if (lDataType == BDIMAGE)
   {
      if (GetRowDateTime(lRow, datetime))
	   {
         m_Grid.GetInteger(lCol, lRow, &lData);
         CImageFile* pImageFileOld = (CImageFile*)(LONG)lData;

         CImageFile* pImageFile = new CImageFile;
         if (pImageFile != NULL)
         {
            // Import the image and delete any previous image
 
            CString sFilter;
            
            CImageFile::GetOpenFilterString(sFilter);

            CFileDialog dlg(TRUE, NULL, "", OFN_FILEMUSTEXIST, sFilter);
            if (dlg.DoModal() == IDOK && pImageFile->Open(dlg.GetPathName(), CImageFile::GeoRef, GetSafeHwnd()))
            {
               m_Grid.SetInteger(lCol, lRow,  (LONG)(LPCSTR)pImageFile);
               if (pImageFileOld != NULL) delete pImageFileOld;
            } else
            {
               delete pImageFile;
            }            

            // Set the color to indicate if data was imported

            m_Grid.GetInteger(lCol, lRow, &lData);
            if (lData != NULL) m_Grid.SetColor(lCol, lRow, BD_BLUE, BD_BLUE);
            else m_Grid.SetColor(lCol, lRow, BD_WHITE, BD_WHITE);
            
         };            
	   };         
   }

   // Support for files embedded in the database

   else if (lDataType == BDFILE)
   {
      CString s = BDString(IDS_ALLFILES) + " (*.*)|(*.*)||";
      CFileDialog dlg(TRUE, NULL, "*.*", OFN_FILEMUSTEXIST, s);
      if (dlg.DoModal() == IDOK)
      {
         m_Grid.GetInteger(lCol, lRow, &lData);
         CFileObj* pFileObjOld = (CFileObj*)(LONG)lData;
         CFileObj* pFileObj = new CFileObj;

         if (pFileObj != NULL && pFileObj->Open(dlg.GetPathName()))
         {
            m_Grid.SetInteger(lCol, lRow,  (LONG)(LPCSTR)pFileObj);
            if (pFileObjOld != NULL) delete pFileObjOld;
         } else
         {
            delete pFileObj;
            AfxMessageBox(BDString(IDS_ERRORREAD) + " " + dlg.GetPathName());
         }
      }
   }

   // Hotlink

   else if (lDataType == BDHOTLINK)
   {
      CString s = BDString(IDS_ALLFILES) + " (*.*)|(*.*)||";
      CFileDialog dlg(TRUE, NULL, "*.*", OFN_FILEMUSTEXIST, s);
      if (dlg.DoModal() == IDOK)
      {
         m_Grid.SetValue(lCol, lRow, dlg.GetPathName());
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::OnExport() 
{
   CDateTime datetime;
   long lRow, lCol;
   long lDataType;
   long lValue;
   CLongLines* pMapLines = NULL;

   m_Grid.GetActiveCell(&lCol, &lRow);
       
   // Export the data

   m_Grid.GetColUserData(lCol, &lDataType);
   if (lDataType == BDMAPLINES)
   {
	   if (GetRowDateTime(lRow, datetime))
	   {                   
         m_Grid.GetInteger(lCol, lRow, &lValue);            		  
         pMapLines = (CLongLines*)lValue;
         if (pMapLines != NULL)
         {				 
#ifdef _DEBUG
               BOOL b = CSpatial::IsPolygon(pMapLines);
#endif

            if (!ExportMapLines(pMapLines))		  
            {
               AfxMessageBox(BDString(IDS_ERROREXPORT));
            }
         };
	   };         
   }
}


///////////////////////////////////////////////////////////////////////////////
//
// Create the grid control
//

BOOL CDlgEditAttributes::CreateGrid() 
{   
	CRect rect;

   GetDlgItem(IDC_GRID)->GetClientRect(&rect);   
   
   BOOL bOK = m_Grid.Create(WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, rect, 
                        this, IDC_GRID1);

   if (bOK)
   {      
      m_Grid.SetUserResize(SS_USERRESIZE_COL);	
	   m_Grid.SetBool(SSB_PROCESSTAB, TRUE);
      m_Grid.SetEditEnterAction(SS_ENTERACTION_NEXT);            
      m_Grid.SetBool(SSB_EDITMODEREPLACE, TRUE);
   }   

   PostMessage(WM_SIZE, SIZE_RESTORED, 0);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgEditAttributes::LoadData(long lFeature, int& iRow)
{
   CAttrArray aAttr;   
   CString sDate;
   CDateTime datetime;
   BOOL bOK = TRUE;   
   int iCol = COL_DATE;   

   BeginWaitCursor();

   // Determine feature name

   CFeature feature;
   feature.m_lFeatureTypeId = m_lFType;
   feature.m_lId = lFeature;
   BDFeature(BDHandle(), &feature, BDSELECT);
   BDEnd(BDHandle());

   // Output the column values and retrieve all subsequent readings

   aAttr.m_lFType = m_lFType;
   aAttr.m_lFeature = lFeature;

   BOOL bFound = BDAttribute(BDHandle(), &aAttr, BDSELECT3);

   do 
   {
      m_Grid.SetMaxRows(iRow);   

      CString s;
      s.Format("%i", feature.m_lId);
      m_Grid.SetValue(COL_FEATURE, iRow,  feature.m_sName);
      m_Grid.SetValue(COL_FEATUREID, iRow, s);            

      if (!bFound) iRow++;

      if (bFound)
      {
         iCol = COL_DATE;      
         datetime = CDateTime(aAttr.m_lDate, 0);   
         datetime.DateAsString(sDate);
      
         m_Grid.SetValue(iCol++, iRow, sDate);            

         int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
         {
            CBDAttribute* pAttr = (CBDAttribute*)aAttr.GetAt(i);                                 

			// Predefined coordinates for creating from map

            m_Grid.SetValue(iCol, iRow, pAttr->AsString());         

		    // For links find the matching value in the combo box

		    if (pAttr->GetDataType() == BDLINK)
		    {
			   int i = 0; for (i = 0; i < m_aFTypeLink[iCol].GetSize(); i++)
			   {
			      if (m_aFTypeLink[iCol][i] == (DWORD)*pAttr->GetLink())
			      {
                 m_Grid.ComboBoxSendMessage(iCol, iRow, CB_SETCURSEL,i,0);
				     break;
			      }
			   }			
		    }

            // For map line data store a pointer to the map line object
 
            if (pAttr->GetDataType() == BDMAPLINES)
            {
			   CLongLines* pMapLines = NULL;
			   if (pAttr->GetLongBinary()->m_hData != NULL)
			   {            
               pMapLines = new CLongLines(*pAttr->GetLongBinary());            

               // Check that the long binary contains valid map lines

               if (pMapLines->GetSize() == 0)
               {
                  delete pMapLines;
                  pMapLines = 0;
               } else
               {
			         m_Grid.SetColor(iCol, iRow, BD_BLUE, BD_BLUE);
               };
			   } 
            m_Grid.SetInteger(iCol, iRow, (LONG)(LPCSTR)pMapLines);            			   
            }

            // For image files, store a pointer to the image object

            CImageFile* pImageFile = NULL;
            if (pAttr->GetDataType() == BDIMAGE)
            {
               pImageFile = new CImageFile;
               if (pImageFile->Initialise(*pAttr->GetLongBinary()))
               {
                  m_Grid.SetColor(iCol, iRow, BD_BLUE,  BD_BLUE);
               } else
               {
                  delete pImageFile;
                  pImageFile = NULL;
               }                             
               m_Grid.SetInteger(iCol, iRow, (LONG)(LPCSTR)pImageFile);            			   
            }

            // For files, store a pointer to the file object

            CFileObj* pFileObj = NULL;
            if (pAttr->GetDataType() == BDFILE)
            {
               pFileObj = new CFileObj;
               if (pFileObj->Initialise(*pAttr->GetLongBinary()))
               {
                  m_Grid.SetColor(iCol, iRow, BD_BLUE,  BD_BLUE);
               } else
               {
                  delete pFileObj;
                  pFileObj = NULL;
               }                             
               m_Grid.SetInteger(iCol, iRow, (LONG)(LPCSTR)pFileObj);            			   

            }


            iCol++;
         };
         iRow++;

         bFound = BDGetNext(BDHandle());
      } 

   } while (bFound);
   BDEnd(BDHandle());

   EndWaitCursor();

   return bOK;
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::InitMapObject()
{
   CDateTime date;
   CString sDate,sCoord;
   CAttrArray aAttr;
   long lData = 0;

   // Set date

   date.AsSystemDate();
   date.DateAsString(sDate);
   m_Grid.SetValue(COL_DATE, 1, sDate);

   // Set coordinate

   BDFTypeAttrInit(BDHandle(), m_lFType, &aAttr);   
   BDEnd(BDHandle());

   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {
      CBDAttribute* pAttr = (CBDAttribute*)aAttr.GetAt(i);                                 

      // Initialise coordinates

      long lCol = COL_DATE+i+1;
      long lRow = 1;

      if (m_pMapLayerObj->GetDataType() == BDCOORD)
      {
         if (pAttr->GetDataType() == BDCOORD)
         {
            CCoord* pCoord = (CCoord*)m_pMapLayerObj->GetMapObject();
            pAttr->SetCoord(*pCoord);
            sCoord = pAttr->AsString();
            m_Grid.SetValue(lCol, lRow, pAttr->AsString());                     
         }
      }

      // Initialise polygons / polylines

      else if (m_pMapLayerObj->GetDataType() == BDMAPLINES)
      {         
         if (pAttr->GetDataType() == BDMAPLINES)
         {           
            // Create new map object that may be deleted

            CLongLines* pLongLines = new CLongLines;
            pLongLines->Copy(*(CLongLines*)m_pMapLayerObj->GetMapObject());
            m_Grid.SetInteger(lCol, lRow, (LONG)(LPCSTR)pLongLines);                                    
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgEditAttributes::InitialiseCols()
{
   SS_CELLTYPE celltype; 
   SS_CELLTYPE celltypeedit; 
   CAttrArray aAttr;
   BOOL bOK = TRUE;
   CFeature feature;
   double dHeight;
   LOGFONT lf;
   HFONT hFont;

   // Set the default font for the grid
   
   memset(&lf,0,sizeof(LOGFONT));
   lf.lfHeight = -11;
   lf.lfPitchAndFamily = 12;   
   lf.lfCharSet = NRDB_CHARSET;
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));         
   hFont = ::CreateFontIndirect(&lf);   

   m_Grid.SetFont(-1,-1,hFont, TRUE);

   // Set title for first column

   CFeatureType ftype;   
   BDFTypeI(BDHandle(), m_lFType, &ftype);
   BDFeatureType(BDHandle(), &ftype, BDSELECT);
   m_Grid.SetValue(COL_FEATURE, ROW_HEADER, ftype.m_sDesc);
   m_Grid.ShowCol(COL_FEATUREID, FALSE);
   m_Grid.SetLock(COL_FEATURE, -1, TRUE);
   
   // Retrieve the attribute data

   BDFTypeAttrInit(BDHandle(), m_lFType, &aAttr);   
   BDEnd(BDHandle());
      
   // Initialise the column headings

   int iCol = COL_DATE;
   int iRow = ROW_HEADER;
   m_Grid.SetValue(iCol++, iRow, BDString(IDS_DATE));      
      
   m_Grid.SetColWidth(-1, 12);

   // Set row height

   m_Grid.GetRowHeight(0, &dHeight);
   m_Grid.SetRowHeight(0, dHeight*2);
   
   // Initialise array for links

   m_aFTypeLink.SetSize(aAttr.GetSize()+5);

   // For each column set titles and data
      
   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {
      CAttribute* pAttr = aAttr.GetAt(i);            
      m_Grid.SetValue(iCol, iRow, pAttr->GetDesc());
      m_Grid.SetColUserData(iCol, pAttr->GetDataType());

	  if (pAttr->GetDataType() == BDTEXT || pAttr->GetDataType() == BDHOTLINK ||
         pAttr->GetDataType() == BDLONGTEXT)
     {        
         m_Grid.SetColWidth(iCol, 20);
         m_Grid.SetTypeEdit(&celltypeedit, ES_STATIC, BD_SHORTSTR-1, SS_CHRSET_CHR, 
                             SS_CASE_NOCASE);           
         m_Grid.SetCellType(iCol, -1, &celltypeedit);
     };

     if (pAttr->GetDataType() == BDTEXT || pAttr->GetDataType() == BDHOTLINK)
     {
         m_Grid.SetColWidth(iCol, 20);         
     }

	  // Set the cell type for map lines
	  
      if (pAttr->GetDataType() == BDMAPLINES || pAttr->GetDataType() == BDIMAGE ||
          pAttr->GetDataType() == BDFILE)
	  {
		 m_Grid.SetLock(iCol, -1, TRUE);
		 m_Grid.SetTypeInteger(&celltype, 0, 0xFFFFFFFF);   
		 m_Grid.SetCellType(iCol, -1, &celltype);
		 m_Grid.SetColor(iCol, -1, BD_WHITE, BD_WHITE);
	  }
    
	  // Set cell type for links

	  if (pAttr->GetDataType() == BDLINK)
	  {         		
// Ability to add to list

       CString sFType;

       sFType += BDString(IDS_ADDNEW); 
       m_aFTypeLink[iCol].Add(-1);  

       feature.m_lFeatureTypeId = pAttr->GetFTypeLink();
       BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
		 while (bFound)
		 {          
			 if (!sFType.IsEmpty()) sFType += "\t";
          feature.m_sName.TrimRight();
			 sFType += feature.m_sName;
			 m_aFTypeLink[iCol].Add(feature.m_lId);			 
			 bFound = BDGetNext(BDHandle());
		 }
		 BDEnd(BDHandle());    
    
       m_Grid.SetTypeComboBox(&celltype, SS_CB_DROPDOWN, sFType);
		 m_Grid.SetCellType(iCol, -1, &celltype);		 
	  }

      iCol++;
   };
   m_Grid.SetMaxCols(iCol-1);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Free any allocated resources
//

void CDlgEditAttributes::OnDestroy() 
{	
   if (IsWindow(m_Grid.GetSafeHwnd()))
   {
      for (int i = 1; i <= m_Grid.GetMaxRows(); i++)
      {
         DeleteRow(i, FALSE);	  
      }   
   };

   CDialog::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgEditAttributes::DeleteRow(int iRow, BOOL bDelete)
{      
   BOOL bDeleted = FALSE;

   for (int iCol = COL_DATE; iCol <= m_Grid.GetMaxCols(); iCol++)
   {
	   DeleteCell(iCol, iRow);            
   };	

   if (bDelete)
   {
      // Only delete the row if it there is still one remaining for the
      // same feature
      
      if (GetFeature(iRow) == GetFeature(iRow-1) || 
          (iRow+1 <= m_Grid.GetMaxRows() && GetFeature(iRow) == GetFeature(iRow+1)))
      {
          m_Grid.DelRow(iRow);
          bDeleted = TRUE;
      }
           
   };

   return bDeleted;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::OnDelete() 
{	   
   SS_CELLCOORD topleft, bottomright;   
   long lRow, lCol;

   BeginWaitCursor();   
   if (!m_Grid.GetSelectBlock(&topleft, &bottomright))
   {
	   m_Grid.GetActiveCell(&lCol, &lRow);   
	   bottomright.Col = topleft.Col = lCol;
	   bottomright.Row = topleft.Row = lRow;
   }
   
  // All rows

   if (topleft.Row == -1)
   {
	   topleft.Row = 1;
	   bottomright.Row = m_Grid.GetMaxRows();
   }

  // All columns

   if (topleft.Col == -1)
   {	 	  
     for (int i = topleft.Row; i <= bottomright.Row; i++)
     {         
        if (DeleteRow(i))
        {
           bottomright.Row--;
           i--;
        };        
     };
  } 

  // Clear cells

  else
  {
     // if cell contains a pointer then delete it

     for (int iCol = max(COL_DATE, topleft.Col); iCol <= bottomright.Col; iCol++)            
     {
        for (int iRow = topleft.Row; iRow <= bottomright.Row; iRow++)
        {
			DeleteCell(iCol, iRow);           
        }
     }
     m_Grid.ClearDataRange(max(COL_DATE, topleft.Col), topleft.Row, 
                           bottomright.Col, bottomright.Row);
   }   

   RedrawWindow();
   
   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::DeleteCell(int iCol, int iRow)
{
   long lDataType, lData;

   m_Grid.GetColUserData(iCol, &lDataType);
   if (lDataType == BDMAPLINES)
   {
      m_Grid.GetInteger(iCol, iRow, &lData);
      CLongLines* pMapLines = (CLongLines*)(LONG)lData; 
      if (pMapLines != NULL)
      {
         delete pMapLines;                  
         m_Grid.SetInteger(iCol, iRow, NULL);
         m_Grid.SetColor(iCol, iRow, BD_WHITE, BD_WHITE);
      }            
   }   
   // Delete image file

   if (lDataType == BDIMAGE)
   {
      m_Grid.GetInteger(iCol, iRow, &lData);
      CImageFile* pImageFile = (CImageFile*)(LONG)lData; 
      if (pImageFile != NULL)
      {
         delete pImageFile;                  
         m_Grid.SetColor(iCol, iRow, BD_WHITE, BD_WHITE);
      }            
   }

   // Delete file objects

   if (lDataType == BDFILE)
   {
      m_Grid.GetInteger(iCol, iRow, &lData);
      CFileObj* pFileObj = (CFileObj*)(LONG)lData; 
      if (pFileObj != NULL)
      {
         delete pFileObj;                  
         m_Grid.SetColor(iCol, iRow, BD_WHITE, BD_WHITE);
      }            
   }

   m_Grid.SetValue(iCol, iRow, "");
}

///////////////////////////////////////////////////////////////////////////////
//
// Validates and saves the data
//

void CDlgEditAttributes::OnOK() 
{
   BOOL bOK = TRUE;
   CDateTime datetime;
   CAttrArray aAttr; 
   
   aAttr.m_lFeature = 0;
   long lFeature;

   BDBeginTrans(BDHandle());

   BeginWaitCursor();

  // Switch off edit mode to save last value

   m_Grid.SetEditMode(FALSE);	

  // Firstly delete all existing values

   ASSERT(m_alFeatures.GetSize() == 1);
   aAttr.m_lFeature = m_alFeatures[0];
   aAttr.m_lFType = m_lFType;

   BOOL bFound = BDAttribute(BDHandle(),  &aAttr, BDDELETEALL);
   BDEnd(BDHandle());

  // Validate each row and add to the database

   for (int iRow = 1; bOK && iRow <= m_Grid.GetMaxRows(); iRow++)
   {
      lFeature = GetFeature(iRow);
      if (lFeature != aAttr.m_lFeature)
      {
         aAttr.m_lFeature = lFeature;
         aAttr.m_lFType = m_lFType;

         BOOL bFound = BDAttribute(BDHandle(),  &aAttr, BDDELETEALL);
         BDEnd(BDHandle());
      }

	   if (GetRowDateTime(iRow, datetime))
	   {
		  aAttr.m_lDate = datetime.GetDateLong();		  

		  int iColError = RetrieveRow(iRow, aAttr);
        if (iColError == 0)
		  {
			  bOK = BDAttribute(BDHandle(),  &aAttr, BDADD);
		  } else
		  {
			  DisplayError(iColError, iRow);			  
			  bOK = FALSE;
		  }
      } 
	   
	   // If date and time are invalid, check that fields are empty

	   else if (!m_Grid.GetValue(COL_DATE, iRow).IsEmpty())
	   {
		  DisplayError(COL_DATE, iRow);			  
		  bOK = FALSE;
	   }	   

     // If row does not contain a date check that it contains no data

      else
      {
         for (int i = COL_DATE; i <= m_Grid.GetMaxCols(); i++)
         {
            if (m_Grid.GetValue(i, iRow) != "")
            {
               DisplayError(i, iRow, BDString(IDS_NODATE));
               bOK = FALSE;
            }
         }            
      }
   }
   BDEnd(BDHandle());
   EndWaitCursor();


   // If all okay then commit otherwise rollback
   if (bOK)
   {
      BDCommit(BDHandle());
	  CDialog::OnOK();
   } else
   {
	  BDRollBack(BDHandle());
   }
}

///////////////////////////////////////////////////////////////////////////////

long CDlgEditAttributes::GetFeature(int iRow)
{
   long lFeature = 0;
   CString s = m_Grid.GetValue(COL_FEATUREID, iRow);
   sscanf(s, "%i", &lFeature);
   return lFeature;
}

///////////////////////////////////////////////////////////////////////////////
//
// Validates and retrieves data into a pre-defined CAttrArray object.  Returns
// the column number containing an error or zero if no error occurred
//

BOOL CDlgEditAttributes::RetrieveRow(int iRow, CAttrArray& aAttr)
{
	int iCol = COL_DATE+1;
	int iRet = 0;
	CString sValue;	
	long lValue;
	CLongLines* pMapLines = NULL;	   
	int iSel = 0;

   aAttr.m_lOrder = iRow;

	int i = 0; for (i = 0; iRet == 0 && i < aAttr.GetSize(); i++)
	{
       CAttribute* pAttr = aAttr.GetAt(i);                        

	   // Retrieve text

	   switch (pAttr->GetDataType())
	   {
	       case BDTEXT : 
		   {
			  sValue = m_Grid.GetValue(iCol, iRow);
			  sValue.TrimLeft(" ");			  
			  pAttr->SetString(sValue);          		  			  
		   }
		   break;

         // Long text

		   case BDLONGTEXT : 
			{
            sValue = m_Grid.GetValue(iCol, iRow);
			   sValue.TrimLeft(" ");			  
			   pAttr->SetLongText(sValue);          	
			}
         break;

         // Get File name

         case BDHOTLINK : 
         {
            sValue = m_Grid.GetValue(iCol, iRow);
            sValue.TrimLeft();            
            pAttr->SetFile(sValue);               
            
         }
         break;

	   // Retrieve numbers
	   
		   case (BDNUMBER) :
		   {
			  sValue = m_Grid.GetValue(iCol, iRow);
           sValue.TrimLeft();
           sValue.TrimRight();

           if (!pAttr->AsAttr(sValue))
           {
				  iRet = iCol;
			  }
		   }
		   break;

		// Retrieve link

		   case (BDLINK) :
		   {
			  iSel = m_Grid.ComboBoxSendMessage(iCol, iRow, CB_GETCURSEL, 0,0);
			  if (iSel != CB_ERR)
			  {
              if (m_aFTypeLink[iCol][iSel] != -1)
              {
				     pAttr->SetLink(m_aFTypeLink[iCol][iSel]);
              } else
              {
                 iRet = iCol;
              }
			  } else
			  {
				  iRet = iCol;
			  }
		   }
		   break;
		   
	   // Retrieve map lines              

		   case (BDMAPLINES) : 
		   {  
			  CLongBinary longbinary; // Re-initialised each iteration

			  m_Grid.GetInteger(iCol, iRow, &lValue);            		  
			  pMapLines = (CLongLines*)lValue;
			  if (pMapLines != NULL)
			  {
				 pMapLines->GetLongBinary(longbinary);		
			  }
			  pAttr->SetLongBinary(&longbinary);
			  longbinary.m_hData = NULL; // prevents deletion of memory
		   }
		   break;

         // Retrieve image

         case BDIMAGE :
         {
            CLongBinary longbinary;
            CImageFile* pImageFile = NULL;

            m_Grid.GetInteger(iCol, iRow, &lValue);
            pImageFile = (CImageFile*)lValue;
            if (pImageFile != NULL)
            {
               pImageFile->AsLongBinary(longbinary);
            }
            pAttr->SetLongBinary(&longbinary);
            longbinary.m_hData = NULL; // prevents deletion of memory         
         }
         break;

         // Retrieve file object

         case BDFILE : 
         {
            CLongBinary longbinary;
            CFileObj* pFileObj = NULL;

            m_Grid.GetInteger(iCol, iRow, &lValue);
            pFileObj = (CFileObj*)lValue;
            if (pFileObj != NULL)
            {
               pFileObj->GetLongBinary(longbinary);
            }
            pAttr->SetLongBinary(&longbinary);
            longbinary.m_hData = NULL; // prevents deletion of memory         
         }
         break;

	   // Retrieve coordinates

		   case BDCOORD :
		   {
			   CCoord coord;
			   sValue = m_Grid.GetValue(iCol, iRow);            
            sValue.TrimLeft();
            sValue.TrimRight();

            if (sValue == "") 
            {
               coord.SetNull();
               pAttr->SetCoord(CCoord(coord.x,coord.y));
            }
			   else if (BDProjection()->StringAsCoord(sValue, &coord))
			   {
				   pAttr->SetCoord(CCoord(coord.x,coord.y));
			   } else
			   {
				  iRet = iCol;
			   }
		   }
		   break;

	   // Retrieve boolena

		   case (BDBOOLEAN) :
		   {
             sValue = m_Grid.GetValue(iCol, iRow);
			 sValue.TrimRight();
			 if (sValue.CompareNoCase(BDString(IDS_YES)) == 0 || sValue.CompareNoCase(BDString(IDS_YES1)) == 0)
			 {
				 pAttr->SetBoolean(TRUE);
			 }
			 else if (sValue.CompareNoCase(BDString(IDS_NO)) == 0 || sValue.CompareNoCase(BDString(IDS_NO1)) == 0)
			 {
				 pAttr->SetBoolean(FALSE);
			 }
			 else if (sValue == "")
			 {
				 pAttr->SetBoolean(AFX_RFX_BOOL_PSEUDO_NULL);
			 }
			 else
			 {
				 iRet = iCol;
			 }

		   }
		   break;

	   default:
	   {
		   ASSERT(FALSE);
	   }
	   };

       iCol++;
	};
	return iRet;
}


///////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::DisplayError(int iCol, int iRow, LPCSTR sError)
{
   CString s;
   long lData;
   CString sMsg;

   if (sError == NULL) sMsg = BDString(IDS_INVALIDVALUE);
   else sMsg = sError;


   m_Grid.GetColUserData(iCol, &lData);
   int i = 0; for (i = 0; i < sizeof(CAttrTypes::m_aAttrType) / sizeof(CAttrTypes); i++)
   {
	   if (lData == CAttrTypes::m_aAttrType[i].m_nId) break;
   }

   if (iCol != COL_DATE)
   {
      // Output type of error

      CString s1 = BDString(IDS_OFTYPE);
      CString s2 = BDString(IDS_ONROW);

      s.Format("%s \"%s\" %s %s %s %i. ", sMsg, m_Grid.GetValue(iCol, 0), 
         (LPCSTR)s1, (LPCSTR)CAttrTypes::GetDesc(lData),(LPCSTR)s2, iRow);
      s += "\r\n\r\n";
 
      // Describe correct format

      switch (CAttrTypes::m_aAttrType[i].m_nId)
      {      
         case BDNUMBER : s += BDString(IDS_NUMFORMAT); break;
         case BDCOORD : s += BDString(IDS_COORDFORMAT); break;
         case BDLINK : s += BDString(IDS_LINKFORMAT); break;
         case BDHOTLINK : s += BDString(IDS_FILEFORMAT); break;
      }   
   } else
   {
      // Output error for dates
      
      s.Format("%s \"%s\" %s %i. ", (LPCSTR)sMsg, m_Grid.GetValue(iCol, 0), 
               (LPCSTR)BDString(IDS_ONROW), iRow);

      s += "\r\n\r\n" + BDString(IDS_DATEFORMAT);    
   }   

   AfxMessageBox(s);
   m_Grid.SetActiveCell(iCol, iRow);
   m_Grid.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CDlgEditAttributes::OnResetProgress(WPARAM wParam, LPARAM lParam)
{
   BDGetProgressBar()->SetRange32(wParam, lParam);
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CDlgEditAttributes::OnUpdateProgress(WPARAM wParam, LPARAM)
{
   BDGetProgressBar()->SetPos(wParam);
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CDlgEditAttributes::OnLeaveCell(WPARAM,LPARAM)
{
   PostMessage(WM_CHANGECELL);
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CDlgEditAttributes::OnChangeCell(WPARAM,LPARAM)
{
   long lRow, lCol, lData, lValue;
   BOOL bData = FALSE;

   m_Grid.GetActiveCell(&lCol, &lRow);   
   GetDlgItem(IDC_STATUS)->SetWindowText(m_Grid.GetValue(lCol, 0));	
   
   // Determine the column type

   m_Grid.GetColUserData(lCol, &lData);

   // Disable if the row contains no date

   BOOL bDate = m_Grid.GetValue(COL_DATE, lRow) != "";

   // Determine if the cell contains data

   if (lData == BDMAPLINES || lData == BDIMAGE || lData == BDFILE)
   {
      m_Grid.GetInteger(lCol, lRow, &lValue);            		  
      bData = lValue != 0;
   };
                
   // Disable import except for map lines
   
   GetDlgItem(IDC_IMPORT)->EnableWindow((lData == BDMAPLINES || lData == BDHOTLINK || lData == BDIMAGE || lData == BDFILE) && bDate);
   GetDlgItem(IDC_EXPORT)->EnableWindow(lData == BDMAPLINES && bDate && bData);
 
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CDlgEditAttributes::OnDataChange(WPARAM,LPARAM)
{
    // If the last row then add a new one

   return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Professional version, can insert new rows for current type
//

void CDlgEditAttributes::OnInsert() 
{
   long lCol, lRow;
   m_Grid.GetActiveCell(&lCol, &lRow);      
   CString sFeature = m_Grid.GetValue(COL_FEATURE, lRow);
   CString sID = m_Grid.GetValue(COL_FEATUREID, lRow);

   if (sID != "")
   {
      //m_Grid.SetMaxRows(m_Grid.GetMaxRows()+1);
      m_Grid.InsRow(lRow+1);      

      m_Grid.SetValue(COL_FEATURE, lRow+1, sFeature);
      m_Grid.SetValue(COL_FEATUREID, lRow+1, sID);
   };     	
}



/////////////////////////////////////////////////////////////////////////////
//
// Provide option for adding new links
//

LRESULT CDlgEditAttributes::OnComboSelChange(WPARAM wParam, LPARAM lParam)
{

   CAttrArray aAttr;
   int index;
   CFeature feature;

   // Retrieve selected link

   long iRow, iCol;
   m_Grid.GetActiveCell(&iCol, &iRow);   
   int iSel = m_Grid.ComboBoxSendMessage(iCol, iRow, CB_GETCURSEL, 0,0);   

   // If link is 'new' then display add dialog

   if (iSel != CB_ERR && m_aFTypeLink[iCol][iSel] == -1)
   {
      BDFTypeAttrInit(BDHandle(), m_lFType, &aAttr);   
      BDEnd(BDHandle());

      long lAttrFType = aAttr.GetAt(iCol-COL_DATE-1)->GetFTypeLink();

      CDlgEditFeature dlg(lAttrFType);
      if (dlg.DoModal() == IDOK)
      {
         feature.m_lId = dlg.GetId();
         feature.m_lFeatureTypeId = lAttrFType;
         BDFeature(BDHandle(), &feature, BDSELECT);
         BDEnd(BDHandle());         

         // Add new string to each list

         m_aFTypeLink[iCol].Add(feature.m_lId);  
                  
         index = m_Grid.ComboBoxSendMessage(iCol, -1, CB_ADDSTRING, 0, (long)(LPCSTR)feature.m_sName);         
            
         // Set active row to new selection

         m_Grid.ComboBoxSendMessage(iCol, iRow, CB_SETCURSEL,m_aFTypeLink[iCol][index],0);                                                         
      }
   }
			  		
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
   // Resize the grid

   if (!IsWindow(m_Grid)) return;
   //if (GetDlgItem(IDC_GRID) == NULL) return;

   CRect rectGrid, rectForm, rect, rectCForm;
   
   GetDlgItem(IDC_GRID)->GetWindowRect(&rectGrid);
   GetWindowRect(&rectForm);
   GetClientRect(&rectCForm);

   int nBorder = (rectForm.Height() - rectCForm.Height());   

   rect.left = rectGrid.left-rectForm.left;
   rect.top = rectGrid.top - rectForm.top - nBorder;
   rect.right = rectForm.Width() - nBorder;
   rect.bottom = rectForm.Height() - nBorder*2;
   
   if (nType == SIZE_MAXIMIZED || nType == SIZE_RESTORED)
   {          
      m_Grid.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
                          SWP_NOOWNERZORDER);      
   };
}

/////////////////////////////////////////////////////////////////////////////

void CDlgEditAttributes::OnClose() 
{
   int nRet = AfxMessageBox(BDString(IDS_SAVECHANGES), MB_YESNOCANCEL);
   if (nRet == IDYES)
   {
      OnOK();
   } 
   else if (nRet == IDNO)
   {	
	   CDialog::OnClose();
   };   
}




void CDlgEditAttributes::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////////////////

BOOL CDlgEditAttributes::PreTranslateMessage(MSG* pMsg) 
{   
	return CDialog::PreTranslateMessage(pMsg);
}
