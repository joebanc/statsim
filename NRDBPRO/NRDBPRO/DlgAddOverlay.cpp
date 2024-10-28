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
#include "dlgprogress.h"
#include "DlgAddOverlay.h"
#include "shapefile.h"
#include "importtextfile.h"
#include "comboboxsymbol.h"
#include "importmaplines.h"
#include "spatial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddOverlay dialog


CDlgAddOverlay::CDlgAddOverlay(CMapLayer* pMapLayer, LPCSTR sFileName, LPCSTR sColumn, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddOverlay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddOverlay)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pMapLayer = pMapLayer;
	m_sPath = sFileName;
	m_sColumn = sColumn;
   m_bDefault = m_sPath != "";
}


void CDlgAddOverlay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddOverlay)
	DDX_Control(pDX, IDC_MAPOBJ, m_cbMapObj);
	DDX_Control(pDX, IDC_FTYPE, m_cbFType);
	DDX_Control(pDX, IDC_COLOR, m_pbColour);
	DDX_Control(pDX, IDC_LABEL, m_cbLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddOverlay, CDialog)
	//{{AFX_MSG_MAP(CDlgAddOverlay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddOverlay message handlers

BOOL CDlgAddOverlay::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CDatabase database;
	CShapeFile shapefile;
	CImportTextFile importtextfile;
	HSTMT hstmt;
	char szColumnName[64];
	SQLINTEGER cbColumnName;
	int retcode;
	int iCol = 1;
   char szPath[_MAX_PATH];

	// Disable unused controls

	GetDlgItem(IDC_FTYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_FEATURE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MAPOBJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELECTALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_DICTIONARY)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELECT)->EnableWindow(FALSE);	
	m_pbColour.SetColour(m_pMapLayer->GetColour());
			
	// Add null label

	int index = m_cbLabel.AddString(BDString(IDS_NONE));						
	m_cbLabel.SetItemData(index, -1);
	m_cbLabel.SetCurSel(index);

	// Retrieve the name of the shapefile

    CString sFilter = BDString(IDS_ALLFILES)+"|*.shp;*.txt;";
    CImageFile::GetOpenFilterString(sFilter, CImageFile::extonly+CImageFile::open);
    sFilter += "Shape Files|*.shp|"+BDString(IDS_POLYLINESFILE) + "|*.txt|";
    CImageFile::GetOpenFilterString(sFilter, CImageFile::open);   
    
    CFileDialog dlg(TRUE, "shp", NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
                    sFilter);

    if (m_sPath != "" || dlg.DoModal() == IDOK)
    {
       // If the file selected is a JPG image then return straight away

       if (CImageFile::IsImageFile(m_sPath) ||
           CImageFile::IsImageFile(dlg.GetPathName()))
       {
          CDialog::OnOK();
          if (m_sPath == "") 
          {
             m_sPath = dlg.GetPathName();
             m_pMapLayer->SetFileName(m_sPath);
             m_pMapLayer->GetMapProp().m_sName = dlg.GetFileTitle();
	          m_pMapLayer->SetColumnName("");                        
             m_pMapLayer->m_prop.m_nSymbol = CComboBoxSymbol::none;

             CMapLayerObj *pMapLayerObj = new CMapLayerObj;
             pMapLayerObj->SetDataType(BDIMAGE);
             CImageFile *pImageFile = new CImageFile;
             pImageFile->Open(m_sPath, CImageFile::GeoRef);
             pMapLayerObj->SetMapObject(pImageFile);
             m_pMapLayer->Add(pMapLayerObj);
          };          
          return FALSE;
       }

	   // Polylines/polygon text file

	   else if (m_sPath.Right(4).CompareNoCase(".txt") == 0 ||
           dlg.GetFileExt().CompareNoCase("txt") == 0)
	   {
		   CDialog::OnOK();
           if (m_sPath == "") 
           {
			   m_sPath = dlg.GetPathName();
			   m_pMapLayer->SetFileName(m_sPath);
		       m_pMapLayer->GetMapProp().m_sName = dlg.GetFileTitle();
		       m_pMapLayer->SetColumnName("");                 
		   };
		   
		   CMapLayerObj *pMapLayerObj = new CMapLayerObj;
		   CSpatialPoly *pSpatialPoly = new CSpatialPoly;
		   if (pMapLayerObj != NULL && pSpatialPoly != NULL && ImportMapLines(*pSpatialPoly, m_sPath))
		   {
           pSpatialPoly->Initialise();
           pMapLayerObj->SetDataType(BDMAPLINES);
			  pMapLayerObj->SetMapObject(pSpatialPoly);
			  m_pMapLayer->Add(pMapLayerObj);           
           m_pMapLayer->m_prop.m_bPolygon = pSpatialPoly->m_bPolygon;
		   } 
		   // Tidy up
		   else
		   {
			  if (pMapLayerObj != NULL) delete pMapLayerObj;
			  if (pSpatialPoly != NULL) delete pSpatialPoly;
		   }		   
		   // Return as no associated label data to select
		   return FALSE;
	   }

		if (m_sPath == "")
		{
			m_sPath = dlg.GetPathName();
			m_pMapLayer->GetMapProp().m_sName = dlg.GetFileTitle();			 
		}

      // Use the short path name as this can be handled by the ODBC driver

      /*GetShortPathName(m_sPath, szPath, sizeof(szPath));
      m_sPath = szPath;*/
      

		m_sDataSource = m_sPath.Mid(m_sPath.ReverseFind('\\')+1);
		m_sDataSource = m_sDataSource.Left(m_sDataSource.ReverseFind('.'));

		// Set labels

		m_cbFType.AddString(m_sDataSource);
		m_cbFType.SetCurSel(0);

		int nShape = shapefile.ImportShapeFileType(m_sPath);
		if (nShape == SHPPolygon || nShape == SHPPolyLine) m_cbMapObj.AddString(BDString(IDS_POLYLINES));
		if (nShape == SHPPoint) m_cbMapObj.AddString(BDString(IDS_POINTS));
		m_cbMapObj.SetCurSel(0);			   
        
      // Create the data source for the corresponding dbf file

      CString sPath = m_sPath;
      sPath = sPath.Mid(0, sPath.ReverseFind('.')+1) + "dbf";		 
	  	
	  if (CImportTextFile::InitDbaseDataSource(sPath, m_sDataSource))     
	  {                 

		// Open the data source

         BeginWaitCursor();

		if (database.Open(m_sDataSource))
		{				
			if (SQLAllocStmt(database.m_hdbc, &hstmt) == SQL_SUCCESS)
			{
				// Get DBase file name
                 				
                GetShortPathName(sPath, szPath, sizeof(szPath));
#ifdef _DEBUG
				int nError = GetLastError();
#endif
				m_sTable = szPath;
				m_sTable = m_sTable.Mid(m_sTable.ReverseFind('\\')+1);
				m_sTable = m_sTable.Left(m_sTable.ReverseFind('.'));

				retcode = SQLColumns(hstmt, NULL, 0, NULL, 0, (UCHAR*)(LPCSTR)m_sTable, SQL_NTS, NULL, 0);  

				if (retcode == SQL_SUCCESS) 
				{   
					SQLBindCol(hstmt, 4, SQL_C_CHAR, szColumnName, sizeof(szColumnName), &cbColumnName);

				  while(SQLFetch(hstmt) == SQL_SUCCESS) 
				  {			   
					 index = m_cbLabel.AddString(szColumnName);						
					 m_cbLabel.SetItemData(index, iCol++);

					 // If column is preselected then choose it

					 if (m_sColumn == szColumnName) 
					 {
						 m_cbLabel.SetCurSel(index);
					 }

				  };         
				};
				SQLFreeStmt(hstmt,SQL_DROP);      
			};     
			database.Close();
		}
		EndWaitCursor();
             					 
	  }		  		
	  else
	  {
		 OnCancel();
	  }		
	} else
	{
		OnCancel();
	}	

	// Choose preselected column

	if (m_bDefault) OnOK();
  

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////
//
// Retrieve data to accompany the shapefile
//
//

void CDlgAddOverlay::OnOK() 
{
   CDatabase database;
   HSTMT hstmt;   
   SDWORD dwLength;
   CString sSQL;   
   CString sLabel;
   int nRet;
   char s[BD_SHORTSTR] = "";
   int iRow = 0;
   BOOL bOK = TRUE;
   double d;   

   CDlgProgress progress;
   progress.SetText(BDString(IDS_IMPORTSHAPEFILE));

   // Load the shapefile

   CShapeFile shapefile;
   
   if (!shapefile.ImportShapeFile(m_pMapLayer, m_sPath))
   {
	   bOK = FALSE;
   }

   // Determine if the shapefile contains polygon or polylines

   if (m_sColumn == "")
   {
      m_pMapLayer->m_prop.m_bPolygon = TRUE;
      int i = 0; for (i = 0; i < m_pMapLayer->GetSize(); i++)
      {
         if (m_pMapLayer->GetAt(i)->GetDataType() == BDMAPLINES)
         {        
            if (!CSpatial::IsPolygon((CLongLines*)m_pMapLayer->GetAt(i)->GetMapObject()))
            {
               m_pMapLayer->m_prop.m_bPolygon = FALSE;

               m_pMapLayer->m_prop.m_nPattern = BS_NULL;
               m_pMapLayer->m_prop.m_crLine = -1;
               break;

            }
         };
      }
   };
				
  // Determine if one columns is selected for a label

   BeginWaitCursor();

   int index = m_cbLabel.GetCurSel();
   if (bOK && m_cbLabel.GetItemData(index) != -1)
   {	  
	  int iCol = 1;
      m_cbLabel.GetLBText(index, sLabel);

      if (database.Open(m_sDataSource))
	  {
         if (SQLAllocStmt(database.m_hdbc, &hstmt) == SQL_SUCCESS)
		 {                          
			 // Construct SQL

             sSQL = "select " + sLabel + " from [" + m_sTable + "]";
			 nRet = SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 
              
			 // Bind column
			 nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, s, BD_SHORTSTR, &dwLength);

			 // Retrieve data

			 while (bOK && SQLFetch(hstmt) == SQL_SUCCESS)
			 {							
				if (iRow < m_pMapLayer->GetSize())
				{					
				   progress.SetPercent((iRow * 100) / m_pMapLayer->GetSize());

				  // Set the value for the map layer

				   CMapLayerObj* pMapLayerObj = m_pMapLayer->GetAt(iRow++);                   
				   pMapLayerObj->SetText(s);

				   if (sscanf(s, "%lf", &d) == 1)
				   {
                      pMapLayerObj->SetValue(d);
				   }
				} else
				{
				   AfxMessageBox(IDS_ERRORSHAPEFILE);
                   bOK = FALSE;
				}

				s[0] = '\0';
			 }	
			 SQLFreeStmt(hstmt,SQL_DROP);      
		 }       
         database.Close();
	  }		  
   }

   EndWaitCursor();
   
   // Store column name and file name

   if (bOK)
   {
	   m_pMapLayer->SetFileName(m_sPath);
	   m_pMapLayer->SetColumnName(sLabel);

      if (m_pMapLayer->m_prop.m_bPolygon) m_pMapLayer->m_prop.m_crFill = m_pbColour.GetColour();
      else m_pMapLayer->m_prop.m_crLine = m_pbColour.GetColour();
   }
   
   // Tidy up

   if (m_sDataSource != "")
   {
	   CImportTextFile::RemoveDbaseDataSource(m_sDataSource);         
   };

   if (bOK) 
   {
	   CDialog::OnOK();
   }
   else
   {	   
	   CDialog::OnCancel();
   }
   
}

