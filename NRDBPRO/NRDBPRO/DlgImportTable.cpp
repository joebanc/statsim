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
#include "DlgImportTable.h"
#include "importmaplines.h"
#include "shapefile.h"
#include "dlgimportcreate2.h"
#include "importtextfile.h"
#include "dlgprogress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgImportTable dialog


CDlgImportTable::CDlgImportTable(CDatabase* pDatabase, CImportTable importtable, 
                                 BOOL bImport, int nFileType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportTable)
	//}}AFX_DATA_INIT

	m_importTable = importtable;
	m_pDatabase = pDatabase;
   m_bImport = bImport;
   m_nFileType = nFileType;
   m_nShape = 0;
   m_pTooltip = NULL;
}

///////////////////////////////////////////////////////////////////////////////

CDlgImportTable::~CDlgImportTable()
{
   if (m_pTooltip != NULL) delete m_pTooltip;
}

void CDlgImportTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportTable)
	DDX_Control(pDX, IDC_ATTRIBUTE, m_lbAttr);
	DDX_Control(pDX, IDC_COLUMN, m_lbColumn);
	DDX_Control(pDX, IDC_FTYPE, m_cbFType);
	DDX_Control(pDX, IDC_COLUMNATTR, m_lbColumnAttr);   
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportTable, CDialog)
	//{{AFX_MSG_MAP(CDlgImportTable)
	ON_CBN_SELCHANGE(IDC_FTYPE, OnSelchangeFtype)
	ON_CBN_SELCHANGE(IDC_ATTRIBUTE, OnSelchangeAttribute)
	ON_LBN_SELCHANGE(IDC_COLUMN, OnSelchangeColumn)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_DICTIONARY, OnDictionary)	
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_AUTOMATCH, OnAutomatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportTable message handlers

BOOL CDlgImportTable::OnInitDialog() 
{
	CDialog::OnInitDialog();	

   SetWindowText(BDString(IDS_IMPORT) + ": " + m_importTable.m_sTableImport);
   ResizeWindow(IDS_FEATURE);
   CenterWindow();   
   
	InitFTypes();	   

   // Restore previous selections

   if (m_importTable.m_sFType != "")
   {
      Restore();
   };

   // If no feature types then go automatically to create

   if (m_cbFType.GetCount() == 1) OnCreate();  

   // Enable tooltips

   BDHwndDialog()=m_hWnd;
 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::Restore()
{
   // Take a copy of the columns

   CArray <CImportColumn,CImportColumn> aColumn;
   aColumn.Copy(m_importTable.m_aColumn);

   // Update column names and attributes

   OnSelect();

   // Restore columns

   m_importTable.m_aColumn.Copy(aColumn);

   // Remove items that have already been selected

   int i = 0; for (i = 0; i < m_importTable.m_aColumn.GetSize(); i++)
   {
      CImportColumn &column = m_importTable.m_aColumn[i];
      int index1 = m_lbColumn.FindString(-1, column.m_sNameImport);
      int index2 = m_lbAttr.FindString(-1, column.m_sNameAttr);
      if (index1 != LB_ERR)
      {
         m_lbColumn.DeleteString(index1);
         m_lbAttr.DeleteString(index2);
      }      
   }
     
   // Initialise list of selected items

   InitImport();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::InitFTypes()
{  
   CString s;
   m_cbFType.Init(BDFTypeSel());

   int index = m_cbFType.InsertString(0, BDString(IDS_IMPORTNEW));
   m_cbFType.SetItemData(index, -1);   
   m_cbFType.SetCurSel(index);

   int i = 0; for (i = 0; i < m_cbFType.GetCount(); i++)
   {
      m_cbFType.GetLBText(i, s);
      if (m_importTable.m_sFType == s)
      {
         m_cbFType.SetCurSel(i);
         break;
      };
   }	   
}

///////////////////////////////////////////////////////////////////////////////
//
// Determine the type of the shapefile being imported, if any
//

void CDlgImportTable::DetermineShapefile()
{
   CString sTable = m_importTable.m_sTableImport;

  // For dBase files, determine if there is a corresponding shapefile

   CString sConnect = m_pDatabase->GetConnect();
   sConnect.MakeUpper();
   if (m_nFileType == CImportDB::shapefile)
   {
      // Create a progress dialog

      CDlgProgress dlgProgress;

      m_importTable.m_sShapeFile = GetFilePath(".shp");      

      CShapeFile shapefile;
      m_nShape = shapefile.ImportShapeFileType(m_importTable.m_sShapeFile);
   };
}

///////////////////////////////////////////////////////////////////////////////
// 
// Retrieve details of all columns for the selected table
//

void CDlgImportTable::InitColumns()
{	
    HSTMT hstmt;
	char szColumnName[64];
	SQLINTEGER cbColumnName;
	int retcode;

   // If DBase then determine the short file name

	CString sTable = m_importTable.m_sTableImport;
   if (m_nFileType == CImportDB::dbase || m_nFileType == CImportDB::shapefile)
   {
      CString sPath = GetFilePath(".dbf");      
      sTable = CImportTextFile::GetShortFileTitle(sPath);
   }

	if (SQLAllocStmt(m_pDatabase->m_hdbc, &hstmt) == SQL_SUCCESS)
	{
		retcode = SQLColumns(hstmt, NULL, 0, NULL, 0, (UCHAR*)(LPCSTR)sTable, SQL_NTS, NULL, 0);  

	    if (retcode == SQL_SUCCESS) 
       {   
	       SQLBindCol(hstmt, 4, SQL_C_CHAR, szColumnName, sizeof(szColumnName), &cbColumnName);
   
          while(SQLFetch(hstmt) == SQL_SUCCESS) 
          {			   
		      int index = m_lbColumn.AddString(szColumnName);
            m_lbColumn.SetItemData(index, GetColumnData(szColumnName));
          };         
       };
	    SQLFreeStmt(hstmt,SQL_DROP);      
	};     

   if (m_nShape == SHPPoint)
   {
       int index = m_lbColumn.InsertString(0, SHAPEFILE_POINTS);
       m_lbColumn.SetItemData(index, GetColumnData(SHAPEFILE_POINTS));          
   }
   if (m_nShape == SHPPolyLine || m_nShape == SHPPolygon)
   {
      int index = m_lbColumn.InsertString(0, SHAPEFILE_POLYLINES);
       m_lbColumn.SetItemData(index, GetColumnData(SHAPEFILE_POLYLINES));          
   };   
}

///////////////////////////////////////////////////////////////////////////////
//
// Initilialise the attributes, corresponding to the feature type, that the
// data will be imported to
//

void CDlgImportTable::InitAttr()
{
	CAttrArray aAttr;
   CFeatureType ftype;
	int index;

   // Initialise

	m_lbAttr.ResetContent();   
   m_importTable.m_sFeature = "";
   m_importTable.m_sParentFeature = "";        
   m_importTable.m_sGrandParentFeature = "";        
   m_importTable.m_sGreatGrandParentFeature = "";

	index = m_cbFType.GetCurSel();
	if (index != LB_ERR)
	{
       long lFType = m_cbFType.GetItemData(index);

     // Retrieve feature name
              
       if (BDFTypeI(BDHandle(), lFType, &ftype))
       {
          m_importTable.m_sFeature = ftype.m_sDesc;
       }

      // Retrieve the parent name
       
       if (BDFTypeParentI(BDHandle(), lFType, &ftype))
       {
          m_importTable.m_sParentFeature = ftype.m_sDesc;

          // Retrieve grand parent name

          if (BDFTypeParentI(BDHandle(), ftype.m_lId, &ftype))
          {
             m_importTable.m_sGrandParentFeature = ftype.m_sDesc;

			 // Retrieve great grand parent name

		     if (BDFTypeParentI(BDHandle(), ftype.m_lId, &ftype))
			 {
				 m_importTable.m_sGreatGrandParentFeature = ftype.m_sDesc;
			 }
		  }
		  
       }
       
                         	  
	  // Retrieve attributes

	   if (BDFTypeAttrInit(BDHandle(), lFType, &aAttr))
	   {
		// Add default values
		   
		   index = m_lbAttr.AddString(IMPORT_DATE);
         m_lbAttr.SetItemData(index, BDDATE);

         index = m_lbAttr.AddString(IMPORT_ID);
         m_lbAttr.SetItemData(index, BDID);

         index = m_lbAttr.AddString(m_importTable.m_sFeature + " [" + BDString(IDS_NAME) + "]");
         m_lbAttr.SetItemData(index, BDFEATURE);         
         if (m_importTable.m_sParentFeature != "")
         {
            index = m_lbAttr.AddString(m_importTable.m_sParentFeature + " [" + BDString(IDS_NAME) + "]");
            m_lbAttr.SetItemData(index, BDPARENTFEATURE);         
         };
         if (m_importTable.m_sGrandParentFeature != "")
         {
            index = m_lbAttr.AddString(m_importTable.m_sGrandParentFeature + " [" + BDString(IDS_NAME) + "]");
            m_lbAttr.SetItemData(index, BDGRANDPARENTFEATURE);
         }
		 if (m_importTable.m_sGreatGrandParentFeature != "")
		 {
			index = m_lbAttr.AddString(m_importTable.m_sGreatGrandParentFeature + " [" + BDString(IDS_NAME) + "]");
            m_lbAttr.SetItemData(index, BDGREATGRANDPARENTFEATURE);
		 }
           
         // Add attributes

		   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
		   {
            // For coordinates, the x and the y are handled separately

            if (aAttr.GetAt(i)->GetDataType() == BDCOORD)
            {
               CString s = aAttr.GetAt(i)->GetDesc();
                              
               if (m_nShape != SHPPoint)
               {
                  index = m_lbAttr.AddString(s + X_COORDS);
		          m_lbAttr.SetItemData(index, MAKELONG(i+1,X_COORD)); 
                  index = m_lbAttr.AddString(s + Y_COORDS);
                  m_lbAttr.SetItemData(index, MAKELONG(i+1,Y_COORD));

                  index = m_lbAttr.AddString(s + LAT_COORDS);
		          m_lbAttr.SetItemData(index, MAKELONG(i+1,LAT_COORD)); 
                  index = m_lbAttr.AddString(s + LON_COORDS);
                  m_lbAttr.SetItemData(index, MAKELONG(i+1,LON_COORD));
               };

               index = m_lbAttr.AddString(s + IMP_COORDS);
               m_lbAttr.SetItemData(index, MAKELONG(i+1,IMP_COORD));

            } 
            else if (aAttr.GetAt(i)->GetDataType() == BDMAPLINES)
            {
               index = m_lbAttr.AddString(aAttr.GetAt(i)->GetDesc() + IMP_MAPLINES);
		       m_lbAttr.SetItemData(index, MAKELONG(i+1, IMP_MAPLINE));
            }
            else
            {
			   index = m_lbAttr.AddString(aAttr.GetAt(i)->GetDesc());
		       m_lbAttr.SetItemData(index, i+1);
            };
		  }
	   }
	};
   m_lbAttr.SetCurSel(0);
}

///////////////////////////////////////////////////////////////////////////////
//
// Display the columns and attributes linked for input
//

void CDlgImportTable::InitImport()
{
   m_lbColumnAttr.ResetContent();

   int i = 0; for (i = 0; i < m_importTable.m_aColumn.GetSize(); i++)
   {
      CImportColumn &column = m_importTable.m_aColumn[i];      
      m_lbColumnAttr.AddString(column.m_sNameImport + " >> " + column.m_sNameAttr);
   }      
}

///////////////////////////////////////////////////////////////////////////////
//
// Automatically match polylines or coordinate attributes to shapefile, if 
// unique
//

void CDlgImportTable::MatchShape()
{
	CString s;
	DWORD dw = 0;   

   // Find attribute matching shapefile

	int i = 0; for (i = 0; i < m_lbAttr.GetCount(); i++)
	{
		dw = m_lbAttr.GetItemData(i);
		if (HIWORD(dw) == IMP_COORD && m_nShape == SHPPoint) break;
		if (HIWORD(dw) == IMP_MAPLINE && (m_nShape == SHPPolyLine || m_nShape == SHPPolygon)) break;
	}

	// If match found then assign to coordinate
   
	if (i < m_lbAttr.GetCount())
	{
      int j = 0; for (j = 0; j < m_lbColumn.GetCount(); j++)
		{
			m_lbColumn.GetText(j, s);

			if (s == SHAPEFILE_POINTS && m_nShape == SHPPoint) 
			{				
				m_lbColumn.SetCurSel(j);
				m_lbAttr.SetCurSel(i);
				break;
			}
			else if (s == SHAPEFILE_POLYLINES && (m_nShape == SHPPolyLine || m_nShape == SHPPolygon)) 
			{			
				m_lbAttr.SetCurSel(i);
				m_lbColumn.SetCurSel(j);
				break;
			}
		}
   } else
   {
      // If no attributes associated then display warning message
      
      if (m_nShape == SHPPoint)
      {
         AfxMessageBox(BDString(IDS_NOCOORDATTR) + " " + BDString(IDS_ADDMAPATTR));
      }
      else if (m_nShape == SHPPolyLine || m_nShape == SHPPolygon)
      {
         AfxMessageBox(BDString(IDS_NOPOLYLINEATTR) + " " + BDString(IDS_ADDMAPATTR));
      }      
   }

	// Associate attributes
	OnSelchangeAttribute();   


}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::OnSelchangeFtype() 
{
   InitAttr();	
}

///////////////////////////////////////////////////////////////////////////////
// 
// Associate the attribute with the column
//

void CDlgImportTable::OnSelchangeAttribute() 
{

}  

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::OnSelchangeColumn() 
{

}

///////////////////////////////////////////////////////////////////////////////
//
// Given the column name, determines which (if any) attribute the column is
// associated with
//

long CDlgImportTable::GetColumnData(LPCSTR sCol)
{   
   CString sAttr;   

   // Search for column name already used

   int i = 0; for (i = 0; i < m_importTable.m_aColumn.GetSize(); i++)
   {
      if (m_importTable.m_aColumn[i].m_sNameImport == sCol)
         break;
   }

   // If found search for the corresponding attribute name

   if (i < m_importTable.m_aColumn.GetSize())
   {
      int j = 0; for (j = 0; j < m_lbAttr.GetCount(); j++)
      {
         m_lbAttr.GetText(j, sAttr);
         if (m_importTable.m_aColumn[i].m_sNameAttr == sAttr)
         {
            return m_lbAttr.GetItemData(j);
         }
      }
   }
  
   return CImportTable::DoNotImport;
}

///////////////////////////////////////////////////////////////////////////////
//
// Freeze the selection of feature type and feature 
//

void CDlgImportTable::OnSelect() 
{
   int i = m_cbFType.GetCurSel();

   if (i != CB_ERR)
   {       
       if (m_cbFType.GetItemData(i) == -1) 
       {
          OnCreate();
          return;
       }
       BeginWaitCursor();

       m_cbFType.GetLBText(i, m_importTable.m_sFType);
       
       GetDlgItem(IDC_SELECT)->EnableWindow(FALSE); 
       GetDlgItem(IDC_DICTIONARY)->EnableWindow(FALSE); 
       GetDlgItem(IDC_FTYPE)->EnableWindow(FALSE);        
       GetDlgItem(IDC_ATTRIBUTE)->EnableWindow(TRUE); 
       GetDlgItem(IDC_COLUMN)->EnableWindow(TRUE); 
       GetDlgItem(IDOK)->EnableWindow(TRUE);        
       ResizeWindow(IDC_EXTENT);
       CenterWindow();

       DetermineShapefile();
       InitAttr();	
       InitColumns();       

       // Clear the list
      
       m_importTable.m_aColumn.RemoveAll();


	   if (m_nShape)
	   {
	      MatchShape();
	   };

       EndWaitCursor();
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Associate the column and the attribute for importing
//

void CDlgImportTable::OnAdd() 
{
   CImportColumn column;

   // Get the currently selected attributes

   int index1 = m_lbColumn.GetCurSel();
   int index2 = m_lbAttr.GetCurSel();
   if (index1 != LB_ERR && index2 != LB_ERR)
   {
      m_lbColumn.GetText(index1, column.m_sNameImport);      
      m_lbAttr.GetText(index2, column.m_sNameAttr);                     

      column.m_lItemData = m_lbColumn.GetItemData(index1);

      // Remove the items from the lists

      m_lbColumn.DeleteString(index1);
      m_lbAttr.DeleteString(index2);

      // Update table

      m_importTable.m_aColumn.Add(column);

      // Now update lists
   
      InitImport();
   }   
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::OnDelete() 
{
   // Remove columns

   int index = m_lbColumnAttr.GetCurSel();
   if (index != LB_ERR)
   {
      CImportColumn &column = m_importTable.m_aColumn[index];
      
      int i = m_lbColumn.AddString(column.m_sNameImport);
      m_lbColumn.SetItemData(i, column.m_lItemData);
      m_lbAttr.AddString(column.m_sNameAttr);

      m_importTable.m_aColumn.RemoveAt(index);

      InitImport();
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Automatically match columns to column names
//

void CDlgImportTable::OnAutomatch() 
{   
   CString sCol, sAttr;   

   BeginWaitCursor();
   int i = 0; for (i = 0; i < m_lbColumn.GetCount(); i++)
   {
	  BOOL bFound = FALSE;
      m_lbColumn.GetText(i, sCol);
      sCol.TrimRight();
      sCol.MakeUpper();

	  // Search for an exact match (excluding postfix)

      int j = 0; for (j = 0; j < m_lbAttr.GetCount(); j++)
      {         
         m_lbAttr.GetText(j, sAttr);         
         sAttr = CQuery::StripBrackets(sAttr);
         sAttr.TrimRight();

         if (sAttr.CompareNoCase(sCol) == 0)
         {
            m_lbColumn.SetCurSel(i);
            m_lbAttr.SetCurSel(j);
            OnAdd();

            if (i > 0) i--;
            break;
			bFound = TRUE;
         }

      }

	  // If no exact match then search on subtext

	  if (!bFound)
	  {
		  int j = 0; for (j = 0; j < m_lbAttr.GetCount(); j++)
		  {         
			 m_lbAttr.GetText(j, sAttr);         
          sAttr.MakeUpper();
			
          TRACE("sCol="+sCol+" sAttr="+sAttr+"\r\n");

			 if (sAttr.Find(sCol) != -1 || sCol.Find(sAttr) != -1)
			 {
				m_lbColumn.SetCurSel(i);
				m_lbAttr.SetCurSel(j);
				OnAdd();

				if (i > 0) i--;
				break;
				bFound = TRUE;
			 }
		  }
	  }

   }
   EndWaitCursor();

	
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::ResizeWindow(int nControl)
{
   CRect rectW, rectC;   
         
   GetWindowRect(rectW);
   GetDlgItem(nControl)->GetWindowRect(rectC);   

   SetWindowPos(NULL,rectW.left,rectW.left,rectW.right - rectW.left, rectC.bottom-rectW.top +5, 
                SWP_NOZORDER);
};

///////////////////////////////////////////////////////////////////////////////
//
// void CDlgImportTable::OnOK() 
//
// Save the selections
//

void CDlgImportTable::OnOK() 
{
   if (Validate()) CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
//
// Same as OnOK except return IDC_NEXT indicating to return to previous
// dialog
//

void CDlgImportTable::OnNext()
{
   // If no fields selection then don't store results

   if (m_importTable.m_aColumn.GetSize() == 0) EndDialog(IDC_NEXT);

   // Otherwise validate and store selections

   if (Validate()) EndDialog(IDC_NEXT);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgImportTable::Validate()
{
   if (m_importTable.m_aColumn.GetSize() > 0)
   {
      int i = 0; for (i = 0; i < m_importTable.m_aColumn.GetSize(); i++)
      {
         CImportColumn &column = m_importTable.m_aColumn[i];
         long lAttr = column.m_lItemData;

         // Check that points are matched with points

         if (column.m_sNameImport == SHAPEFILE_POINTS)
         {
            // Check that shapefile columns are linked with appropriate types
            
            if (HIWORD(lAttr) != IMP_COORD && 
                lAttr != CImportTable::DoNotImport)
            {
               AfxMessageBox(BDString(IDS_SHAPEFILECOL));
               m_lbColumn.SetCurSel(i);
               OnSelchangeColumn();        
               return FALSE;
            } else
            {
               column.m_bShapeFile = TRUE;
            }
         }

         // Check that polylines are matched with polylines

         if (column.m_sNameImport == SHAPEFILE_POLYLINES)
         {
            // Check that shapefile columns are linked with appropriate types

            if (HIWORD(lAttr) != IMP_MAPLINE && lAttr != CImportTable::DoNotImport)
            {
               AfxMessageBox(BDString(IDS_SHAPEFILECOL));
               m_lbColumn.SetCurSel(i);
               OnSelchangeColumn();        
               return FALSE;
            } else
            {
               column.m_bShapeFile = TRUE;
            }
         }    
      };

      return TRUE;
   };
   return FALSE;

}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::OnDictionary() 
{
   m_cbFType.OnClickDictionary();	
}

///////////////////////////////////////////////////////////////////////////////
//
// To automatically create feature display dialog where user can select matching
// parent features
//

void CDlgImportTable::OnCreate() 
{
   DetermineShapefile();

   // Don't display this window underneath

   ShowWindow(SW_HIDE);

   CDlgImportCreate2 dlg(m_pDatabase, &m_importTable, m_nShape, m_nFileType);
   if (dlg.DoModal() == IDOK)
   {      
      CDialog::OnOK();
   } else
   {
      EndDialog(IDCANCEL);
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Determine the file being imported
//

CString CDlgImportTable::GetFilePath(LPCSTR sExt)
{
   CString sPath;

   int i = m_pDatabase->GetConnect().Find("DefaultDir=");      
   sPath = m_pDatabase->GetConnect().Mid(i+11);
   sPath = sPath.Left(sPath.Find(";"));

   if (sPath[sPath.GetLength()-1] != '\\') sPath += '\\';   
   sPath += m_importTable.m_sTableImport + sExt;

   return sPath;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::PostNcDestroy() 
{
   BDHwndDialog() = NULL;
   BDToolTip() = NULL;
   
	CDialog::PostNcDestroy();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportTable::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	CDialog::OnMouseMove(nFlags, point);
}

