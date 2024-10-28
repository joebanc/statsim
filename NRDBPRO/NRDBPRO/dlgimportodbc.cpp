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
#include "odbcinst.h"
#include "nrdb.h"
#include "DlgImportODBC.h"
#include "importodbc.h"
#include "dlgimporttable.h"
#include "dlgprogress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportODBC dialog


CDlgImportODBC::CDlgImportODBC(LPCSTR sDataSource, LPCSTR sTable, BOOL bSysTables, int nFileType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportODBC::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportODBC)
	//}}AFX_DATA_INIT	

   m_sDatabase = sDataSource;
   m_sTable = sTable;
   m_nFileType = nFileType;
   m_bSysTables = bSysTables;
   m_bNRDB = FALSE;
}


void CDlgImportODBC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportODBC)
	DDX_Control(pDX, IDC_SYSTABLES, m_pbSysTables);
	DDX_Control(pDX, IDC_TABLES, m_lbTables);
	DDX_Control(pDX, IDC_DATASOURCE, m_cbDataSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportODBC, CDialog)
	//{{AFX_MSG_MAP(CDlgImportODBC)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDDEFINE, OnDefine)
	ON_LBN_DBLCLK(IDC_TABLES, OnDblclkTables)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_LBN_SELCHANGE(IDC_TABLES, OnSelchangeTables)
	ON_BN_CLICKED(IDC_ADMINISTRATOR, OnAdministrator)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportODBC message handlers

BOOL CDlgImportODBC::OnInitDialog() 
{		
   CDialog::OnInitDialog();	

   // Set window size

   ResizeWindow(IDC_SYSTABLES);
   CenterWindow();

   BeginWaitCursor();
   InitDataSources();
   EndWaitCursor();
    	             	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::InitDataSources()
{
    HENV henv;
    UCHAR sDSN[SQL_MAX_DSN_LENGTH+1];
    UCHAR sDescription[255];
    SWORD wLenDSN, wLenDesc;	
    BOOL bFound = FALSE;

    m_cbDataSource.ResetContent();

    // Load the list of data sources into the combo box
   
   if (SQLAllocEnv(&henv) == SQL_SUCCESS)
   {
      SWORD wDirection = SQL_FETCH_FIRST;         
      while (SQLDataSources(henv, wDirection, sDSN, sizeof(sDSN), &wLenDSN, sDescription, sizeof(sDescription),
                     &wLenDesc) == SQL_SUCCESS)
      {
         int index = m_cbDataSource.AddString((LPSTR)sDSN);         
         wDirection = SQL_FETCH_NEXT;         

         if (sDSN == m_sDatabase)
         {
            m_cbDataSource.SetCurSel(index);
            bFound = TRUE;
         }
      }
      SQLFreeEnv(henv);
   };

   // Set the default check value

   m_pbSysTables.SetCheck(m_bSysTables);

   // If a data source is selected then open it

   if (bFound)
   {
      OnConnect();
   }


   if (m_cbDataSource.GetCurSel() == CB_ERR)
   {
      m_cbDataSource.SetCurSel(m_cbDataSource.GetCount()-1);
   };
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnAdministrator() 
{
   SQLManageDataSources(AfxGetMainWnd()->GetSafeHwnd());	
   InitDataSources();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnConnect() 
{
   HSTMT hstmt;
   char sName[64];
   SDWORD dwLength;
   int iTable = LB_ERR;

	int index = m_cbDataSource.GetCurSel();

	// Connect to the database

	if (index != CB_ERR)
	{
		m_cbDataSource.GetWindowText(m_sDatabase);
	   if (m_database.Open(m_sDatabase))
		{
         GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
         GetDlgItem(IDC_ADMINISTRATOR)->EnableWindow(FALSE);
		   GetDlgItem(IDC_DATASOURCE)->EnableWindow(FALSE);
         GetDlgItem(IDC_SYSTABLES)->EnableWindow(FALSE);
		   GetDlgItem(IDOK)->EnableWindow(TRUE);
		   GetDlgItem(IDDEFINE)->EnableWindow(TRUE);
		   GetDlgItem(IDC_LOAD)->EnableWindow(TRUE);
		   GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		   GetDlgItem(IDC_TABLES)->EnableWindow(TRUE);		   

         ResizeWindow(IDOK);
		}
	};	

	 // Load the available tables

    BOOL bSysTables = m_pbSysTables.GetCheck();
	 CString sTable = "TABLE,VIEW";
    if (bSysTables) sTable += ",SYSTEM TABLE";

    if (SQLAllocStmt(m_database.m_hdbc, &hstmt) == SQL_SUCCESS)
	 {
       int retcode = SQLTables(hstmt, NULL, 0, NULL, 0, NULL, 0, 
                               (UCHAR*)sTable.GetBuffer(0), sTable.GetLength());        

	    if (retcode == SQL_SUCCESS) 
		 {   
		   SQLBindCol(hstmt, 3, SQL_C_CHAR, sName, sizeof(sName), &dwLength);   		   
		            
		   while(SQLFetch(hstmt) == SQL_SUCCESS) 
		   {			   
			 // Check the table contains columns only if not looking for a specific table (optimization)

			if (m_sTable != "" || ValidColumns(sName)) 
         {
			      int index = m_lbTables.AddString(sName);			            
               m_lbTables.SetSel(index, TRUE);

               if (strcmpi(sName,  m_sTable) == 0)
               {
                  iTable = index;
                  m_lbTables.SetCaretIndex(index);
               }
            };
            if (strcmp(sName,"BDMain") == 0)
            {
               m_bNRDB = TRUE;
            }
		   };         
		 };
		SQLFreeStmt(hstmt,SQL_DROP);            
	 };     
    if (iTable == LB_ERR) m_lbTables.SetCaretIndex(0);
    else m_lbTables.SetCaretIndex(iTable);

   // Import natural resources database

       if (m_bNRDB)
       {
          InitFTypes();        
       } 
       else
       {

	 // Load the tables into the format list

          m_importDB.RemoveAll();
	       int i = 0; for (i = 0; i < m_lbTables.GetCount(); i++)
	       {
	          CImportTable table;
               //table.m_lId = m_lbTables.GetItemData(i);
		       m_lbTables.GetText(i, table.m_sTableImport);
		       m_importDB.Add(table);          
	       };
       }    	 

  // If one table is selected then, unselect all the others

    if (!m_bNRDB && (iTable != LB_ERR || m_lbTables.GetCount() == 1))
    {
       int i = 0; for (i = 0; i < m_lbTables.GetCount(); i++)
       {
          if (i != m_lbTables.GetCaretIndex())
          {
             m_lbTables.SetSel(i, FALSE);
          }
       }

       // Import single table

       int nRet = Define();
       if (nRet == IDOK)
       {

       // Import directly

          OnOK();
       } else if (nRet == IDC_NEXT)
       {
          // Allow user to edit next
       }       
       else
       { 
          OnCancel();
       }
    };


    // Update the define button

    OnSelchangeTables();               

}

///////////////////////////////////////////////////////////////////////////////
//
// Checks that the table name contains more than one column i.e. to identify
// empty Excel sheets
//

BOOL CDlgImportODBC::ValidColumns(LPCSTR sTable)
{
   HSTMT hstmt;
	char szColumnName[64];
	SQLINTEGER cbColumnName;
	int retcode;
   int nCol = 0;

	if (SQLAllocStmt(m_database.m_hdbc, &hstmt) == SQL_SUCCESS)
	{
		retcode = SQLColumns(hstmt, NULL, 0, NULL, 0, (UCHAR*)sTable, SQL_NTS, NULL, 0);  

	    if (retcode == SQL_SUCCESS) 
       {   
	       SQLBindCol(hstmt, 4, SQL_C_CHAR, szColumnName, sizeof(szColumnName), &cbColumnName);
   
          while(SQLFetch(hstmt) == SQL_SUCCESS) 
          {			   
		      nCol++;
          };         
       };
	    SQLFreeStmt(hstmt,SQL_DROP);      
	};     

   return nCol > 1;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnClose() 
{
	if (m_database.IsOpen())
	{
        m_database.Close();
	}
	
	CDialog::OnClose();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnImport() 
{
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnDefine() 
{
   if (Define() == IDOK)
   {
      OnOK();
   }
}

///////////////////////////////////////////////////////////////////////////////

int CDlgImportODBC::Define()
{
   CString sTable;
   int i = m_lbTables.GetCaretIndex();

   if (i != LB_ERR)
   {
      m_lbTables.GetText(i, sTable);
      int index = m_importDB.Find(sTable);
      
      if (index != -1)
      {
         BOOL bImport = m_sTable != "";
         CDlgImportTable dlg(&m_database, m_importDB.GetAt(index), bImport, m_nFileType);
         int nRet = dlg.DoModal();
         if (nRet == IDOK || nRet == IDC_NEXT)
         {
	         m_lbTables.SetSel(i, TRUE);
            m_importDB.SetAt(i, dlg.GetImportTable());

            return nRet;
         }	
      };
   };
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnDblclkTables() 
{
	//OnDefine();	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::ResizeWindow(int nControl)
{
   CRect rectW, rectC;   
         
   GetWindowRect(rectW);
   GetDlgItem(nControl)->GetWindowRect(rectC);   

   SetWindowPos(NULL,rectW.left,rectW.left,
                rectW.right - rectW.left, rectC.bottom-rectW.top +rectC.Height()/2, 
                SWP_NOZORDER);
};

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnSave() 
{
   CFileDialog dlg(FALSE, "nri", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                   "NRDB Import Format|*.nri||");
   if (dlg.DoModal() == IDOK)
   {
      if (Validate())
      {
	      m_importDB.Write(dlg.GetPathName());
      };
   };
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnLoad() 
{
   BOOL bOK = TRUE;

   TRY
   {
	   CFileDialog dlg(TRUE, "nri", NULL, OFN_HIDEREADONLY, 
                      "NRDB Import Format|*.nri||");
      if (dlg.DoModal() == IDOK)
      {    
	      if (m_importDB.Read(dlg.GetPathName()))
         {
            // Restore selections

            int i = 0; for (i = 0; i < m_importDB.GetSize(); i++)
            {
               int index = m_lbTables.FindStringExact(-1, m_importDB.GetAt(i).m_sTableImport);
               if (index != LB_ERR)
               {
                  m_lbTables.SetSel(index, m_importDB.GetAt(i).m_bImport);
               }
            }
         } else
         {
            bOK = FALSE;
         }            
      };

      // Load the tables not in format

       CString sTable;	 
	    int i = 0; for (i = 0; i < m_lbTables.GetCount(); i++)
	    {
          m_lbTables.GetText(i, sTable);    
          int index = m_importDB.Find(sTable);
          if (index == -1)
          {
             CImportTable table;
		       table.m_sTableImport = sTable;
		       m_importDB.Add(table);          
          }
	    };	
       OnSelchangeTables();
   }

   // Error handling

   CATCH (CFileException, pEx)
   {
      bOK = FALSE;
   } END_CATCH

   if (!bOK)
   {
      AfxMessageBox(BDString(IDS_ERRORFORMAT));
      m_importDB = CImportDB();
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// void CDlgImportODBC::OnOK() 
//

void CDlgImportODBC::OnOK() 
{
   if (m_lbTables.GetSelCount() == 0)
   {
      AfxMessageBox(BDString(IDS_NOTABLESSEL));
      return;
   }   

	if (Validate())
   {
      // Autosave      
	   CDialog::OnOK();
   };
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgImportODBC::Validate()
{
      CString sTable;

  // For each table determine if it shall be imported
   
   int i = 0; for (i = 0; i < m_lbTables.GetCount(); i++)
   {      
      m_lbTables.GetText(i, sTable);
      int index = m_importDB.Find(sTable);
      ASSERT(index != -1);

      CImportTable table = m_importDB.GetAt(index);
      table.m_bImport = m_lbTables.GetSel(i);
      m_importDB.SetAt(index, table);
      
      // For each selected table ensure that its columns are defined

      if (m_importDB.GetAt(index).m_bImport && !m_bNRDB)
      {
         if (m_importDB.GetAt(index).m_aColumn.GetSize() == 0)
         {
            AfxMessageBox(BDString(IDS_DEFINEIMPORT));
            m_lbTables.SetCurSel(i);
            m_lbTables.SetFocus();
            return FALSE;
         }
      }
      
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnSelectall() 
{
	BOOL bSelect = m_lbTables.GetCount() != m_lbTables.GetSelCount();
   int i = 0; for (i = 0; i < m_lbTables.GetCount(); i++)
   {
      m_lbTables.SetSel(i, bSelect);
   }	
   OnSelchangeTables();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportODBC::OnSelchangeTables() 
{
   BOOL bEnable = FALSE;
   int index = m_lbTables.GetCaretIndex();
   if (index != LB_ERR)
   {
      bEnable = m_lbTables.GetSel(index);
   }
   GetDlgItem(IDDEFINE)->EnableWindow(bEnable && !m_bNRDB);
   GetDlgItem(IDC_LOAD)->EnableWindow(!m_bNRDB);
   GetDlgItem(IDC_SAVE)->EnableWindow(!m_bNRDB);
}

///////////////////////////////////////////////////////////////////////////////
//
// If a natural resources database has been selected then display its feature
// types
//

void CDlgImportODBC::InitFTypes()
{   
   HSTMT hstmt;
   CString sSQL;
   int nRet;
   int iCol = 1;
   SDWORD dwLength;
   char sDesc[BD_SHORTSTR];
   char sInternal[BD_SHORTSTR];
   long lFTypeLink;

   CDlgProgress dlgProgress;

   CFeatureType ftype;
   CArray <CFeatureType, CFeatureType> m_aFType;
   CAttrArray aAttr;   

   m_lbTables.ResetContent();

   // Connect to the database

   if (SQLAllocStmt(m_database.m_hdbc, &hstmt) == SQL_SUCCESS)   
   {         
      sSQL = "select `ID`, `Description`, `Internal`, `Parent_FType`, `ManyToOne`, `Dictionary` from `BDFeature_Types`";
      nRet = SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 

      nRet = SQLBindCol(hstmt, 1, SQL_C_LONG, &ftype.m_lId, sizeof(long), &dwLength); 
      nRet = SQLBindCol(hstmt, 2, SQL_C_CHAR, sDesc, BD_SHORTSTR, &dwLength); 
      nRet = SQLBindCol(hstmt, 3, SQL_C_CHAR, sInternal, BD_SHORTSTR, &dwLength); 
      nRet = SQLBindCol(hstmt, 4, SQL_C_LONG, &ftype.m_lParentFType, sizeof(long), &dwLength); 
      nRet = SQLBindCol(hstmt, 5, SQL_C_SHORT, &ftype.m_bManyToOne, sizeof(short), &dwLength); 
      nRet = SQLBindCol(hstmt, 6, SQL_C_LONG, &ftype.m_lDictionary, sizeof(long), &dwLength); 
      
      while (SQLFetch(hstmt) == SQL_SUCCESS)
      {
         ftype.m_sDesc = sDesc;
         ftype.m_sInternal = sInternal;
         m_aFType.Add(ftype);
      }

      SQLFreeStmt(hstmt,SQL_DROP);            
   }

   // Order the list of feature types so that linked items appear first

   int i = 0; for (i = 0; i < m_aFType.GetSize(); i++)
   {
      dlgProgress.SetPercent((i*100) / m_aFType.GetSize());      

      SQLAllocStmt(m_database.m_hdbc, &hstmt);
      sSQL.Format("select `FType_Link` from `BDFtype_Attr` where `Feature_Type_ID` = %d"
                " and `Data_Type_ID` = %d", m_aFType[i].m_lId, BDLINK);
	  SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 
      SQLBindCol(hstmt, 1, SQL_C_LONG, &lFTypeLink, sizeof(long), &dwLength); 

      while (SQLFetch(hstmt) == SQL_SUCCESS)
      {
         for (int k = 0; k < m_aFType.GetSize(); k++)
         {
            if (m_aFType[k].m_lId == lFTypeLink)
            {
               // Move the linked item before the item

               if (k > i)
               {
                  ftype = m_aFType[k];
                  m_aFType.RemoveAt(k);
                  m_aFType.InsertAt(i, ftype);
               }
            }
         }                 
      }
	  SQLFreeStmt(hstmt,SQL_DROP); 
   };

   // Add to the listbox and update format list using database table names

   m_importDB.RemoveAll();
   for (i = 0; i < m_aFType.GetSize(); i++)
   {
      int index = m_lbTables.AddString(m_aFType[i].m_sDesc);
      m_lbTables.SetItemData(index, m_aFType[i].m_lId);

      CImportTable table;
      table.m_ftype = m_aFType[i];
      table.m_sTableImport = m_aFType[i].m_sDesc;
      m_importDB.Add(table);          
      
   }   
}
