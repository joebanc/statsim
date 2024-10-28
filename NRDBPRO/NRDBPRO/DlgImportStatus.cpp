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
#include <fcntl.h>

#include "nrdb.h"
#include "DlgImportStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define MAX_LEN_95 64000

/////////////////////////////////////////////////////////////////////////////
// CDlgImportStatus dialog


CDlgImportStatus::CDlgImportStatus(LPCSTR sLogFile, BOOL bSave, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_sLogFile = sLogFile;
   m_bSave = bSave;
}


void CDlgImportStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportStatus)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportStatus, CDialog)
	//{{AFX_MSG_MAP(CDlgImportStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportStatus message handlers

BOOL CDlgImportStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   // Load the data into the edit control from the file

   DWORD dwLength = GetFileLength(m_sLogFile);

   // For Windows 95/98/ME restrict to 64K
   
   DWORD dw = GetVersion();
   BOOL bWinNT = dw < 0x80000000;
   if (!bWinNT) dwLength = min(MAX_LEN_95, dwLength);

   if (dwLength > 0)
   {
      char* pData = new char[dwLength+1];
      if (pData != NULL)
      {         
         FILE* pFile = fopen(m_sLogFile,"rb");
         fread(pData, sizeof(char), dwLength, pFile);
         fclose(pFile);         

         if (!bWinNT && dwLength == MAX_LEN_95)
         {
            char s[] = "\r\n[cont...]";
            memcpy(pData + dwLength-sizeof(s), s, sizeof(s));
         }

         pData[dwLength] = '\0';
         GetDlgItem(IDC_STATUS)->SetWindowText(pData);
         delete [] pData;         
      }
   }     	

   // Disable save if cancel was pressed

   if (!m_bSave) GetDlgItem(IDOK)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////
//
// long CDlgImportStatus::GetFileLength(CString strFileName)
//
// Determine the length of the given file, returns zero if the file cannot be 
// opened
//

long CDlgImportStatus::GetFileLength(CString strFileName)
{
   int fh; 
   
  // Open the named file and determine its length
   
   if( (fh = _open( strFileName, _O_RDONLY|_O_BINARY))  == -1 )
   {
      return 0;                                                          
   }
   else 
   {  
      long lFileLength = _filelength(fh);      
      _close(fh);
      
      return lFileLength;
   }   
}
