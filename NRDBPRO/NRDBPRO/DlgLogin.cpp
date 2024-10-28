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
#include <odbcinst.h>
#include <io.h>

#include "nrdb.h"
#include "DlgLogin.h"
#include "dlgdbname.h"
#include "importtextfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin dialog


CDlgLogin::CDlgLogin(LPCSTR sDataSource, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLogin)
	//}}AFX_DATA_INIT
   
	m_sDataSource = sDataSource;
}


void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLogin)
	DDX_Control(pDX, IDC_DATASOURCE, m_cbDataSources);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	//{{AFX_MSG_MAP(CDlgLogin)
	ON_BN_CLICKED(IDC_ODBCADM, OnOdbcadm)
   ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin message handlers

BOOL CDlgLogin::OnInitDialog() 
{
	CDialog::OnInitDialog();
	   
   InitDataSources();

   GetDlgItem(IDOK)->SetWindowText(BDString(IDS_OPEN));
   if (BDIsMySQL(BDHandle()))
   {
      GetDlgItem(IDC_ODBCADM)->EnableWindow(FALSE);
	  GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgLogin::InitDataSources()
{
   HANDLE henv = NULL;
   CString strDSN;   
   int index;
     
   CString sDefault = AfxGetApp()->GetProfileString("Database","Default");
   
   m_cbDataSources.ResetContent();

  // Load the list of data sources into the combo box
                     
   while (BDGetDataSourceNames(henv, strDSN, BDGetUser(), BDGetPassword()))
   {   
      CString sDSN = strDSN;
      if (strDSN.Find(BDString(IDS_NRDBCAPS)) == 0 ||
          strDSN.Find(BDString(IDS_NRDBSM)) == 0)
      {  
// Strip prefix nrdb
         
         strDSN = strDSN.Mid(4);             

         index = m_cbDataSources.AddString(strDSN);
         m_cbDataSources.SetItemData(index, 0);
      };

      
      if (sDSN.CompareNoCase(sDefault) == 0) m_cbDataSources.SetCurSel(index);

	 // If the data source is the default data source name specified then
	 // select automatically

	 if (sDSN.CompareNoCase(m_sDataSource) == 0) 
	 {
		 m_cbDataSources.SetCurSel(index);         
		 OnOK();
	 };

   }   

   // Add 'browse data source'

   /*if (!BDIsMySQL(BDHandle()))
   {
      index = m_cbDataSources.AddString("[" + BDString(IDS_BROWSE) + "...]");
      m_cbDataSources.SetItemData(index, -1);
   };*/

   if (m_cbDataSources.GetCurSel() == CB_ERR)
   {
      m_cbDataSources.SetCurSel(m_cbDataSources.GetCount()-1);   
   };

}

/////////////////////////////////////////////////////////////////////////////

void CDlgLogin::OnOdbcadm() 
{
   SQLManageDataSources(AfxGetMainWnd()->GetSafeHwnd());	
   InitDataSources();
}

////////////////////////////////////////////////////////////////////////////

void CDlgLogin::OnNew()
{
   CString sDSN;
   CImportTextFile import;
   BOOL bOK = FALSE;

   if (BDIsMySQL(BDHandle()))
   {
      CDlgDBName dlg;
      if (dlg.DoModal())
      {
         sDSN = dlg.GetDBName();
         if (sDSN.Left(4).CompareNoCase("NRDB") != 0)
         {
            sDSN = "NRDB" + sDSN;
         }
         if (BDCreateDatasource(sDSN, NULL, BDGetUser(), BDGetPassword()))
         {
            if (BDInitialise(sDSN, BDGetUser(), BDGetPassword()))
            {               
               m_sDataSource = sDSN;
               InitDataSources();
               bOK = TRUE;
            }
         }

         // Display error on failure
         if (!bOK)
         {           
            AfxMessageBox("An error occurred trying to create the database");
      
         }
      }

   } else
   {
      CFileDialog dlg(FALSE, "mdb", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                      BDString(IDS_ACCESSFILE) + " (*.mdb)|*.mdb||");

      if (dlg.DoModal() == IDOK)
      {         
         CString sDSN;
         if (dlg.GetFileTitle().Left(4).CompareNoCase(BDString(IDS_NRDBCAPS)) != 0)
         {
            sDSN = BDString(IDS_NRDBCAPS);
         }
         sDSN += dlg.GetFileTitle();

         // Create database

         if (SQLConfigDataSource(GetSafeHwnd(), ODBC_ADD_DSN, BDString(IDS_ACCESSDRIVER), 
                             "CREATE_DB=\"" + dlg.GetPathName() + "\" General"))
         {
            if (import.InitDataSource(BDString(IDS_ACCESSDRIVER), dlg.GetPathName(), 
                                sDSN,  ""))         
            {
               if (BDInitialise(sDSN))
               {               
                  m_sDataSource = sDSN;
                  InitDataSources();
                  bOK = TRUE;
               }
            }                  
         }      

         // Display error on failure
         if (!bOK)
         {           
            AfxMessageBox(IDS_ODBCERROR);
      
         }
      };
   }
}

////////////////////////////////////////////////////////////////////////////

void CDlgLogin::OpenDataSource()
{
   CFileDialog dlg(TRUE, "mdb", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                   BDString(IDS_ACCESSFILE) + " (*.mdb)|*.mdb||");

   if (dlg.DoModal() == IDOK)
   {
      CString sDSN;
      CreateDataSource(dlg, sDSN);
   }
}

////////////////////////////////////////////////////////////////////////////

BOOL CDlgLogin::CreateDataSource(CFileDialog& dlg, CString& sDSN)
{
   BDHANDLE hConnect;
   CBDMain main;
   BOOL bOK = FALSE;   
      
 // Create the data source

   BeginWaitCursor();

   CImportTextFile import;
   
   // Only add the 'nrdb' prefix to the data source name if its not included in the
   // file name

   if (dlg.GetFileTitle().Left(4).CompareNoCase(BDString(IDS_NRDBCAPS)) != 0)
   {
      sDSN = BDString(IDS_NRDBCAPS);
   }
   sDSN += dlg.GetFileTitle();
    
   if (import.InitDataSource(BDString(IDS_ACCESSDRIVER), dlg.GetPathName(), 
                             sDSN,  ""))
   {
      // Check that this is a valid NRDB database

      BOOL bValid = FALSE;
      
      if (BDConnect(sDSN, &hConnect) == TRUE) 
      {                  
         bValid = BDMain(hConnect, &main, BDGETINIT);
         BDEnd(hConnect);
         BDDisconnect(hConnect);
      }

      // If not valid then remove the data source

      if (!bValid)
      {
         AfxMessageBox(BDString(IDS_INVALIDNRDB) + ": " + sDSN);
         import.RemoveDataSource(BDString(IDS_ACCESSDRIVER), sDSN);             
      } else
      {
         bOK = TRUE;
      }
      if (bOK)
      {
         AfxGetApp()->WriteProfileString("Database","Default", sDSN);
         m_sDataSource = sDSN;
         InitDataSources();
      };
   }
   EndWaitCursor();

   return bOK;
}

////////////////////////////////////////////////////////////////////////////

void CDlgLogin::OnOK() 
{
   int index = m_cbDataSources.GetCurSel();
   if (index != CB_ERR)
   {      
      DWORD dw = m_cbDataSources.GetItemData(index);
      /*if (dw == -1)
      {
         OpenDataSource();
      }
      else if (dw == -2)
      {
         OnNew();
      }
      else*/
      {
         m_cbDataSources.GetWindowText(m_sDataSource);

   // Add 'nrdb' prefix

        m_sDataSource = BDString(IDS_NRDBCAPS) + m_sDataSource;       

         AfxGetApp()->WriteProfileString("Database","Default",m_sDataSource);
	      CDialog::OnOK();
      }             
   };
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLogin::OnSetFont(CFont* pFont) 
{	
	CDialog::OnSetFont(pFont);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLogin::OnBrowse() 
{
	OpenDataSource();
}
