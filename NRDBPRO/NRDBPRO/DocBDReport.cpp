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
#include "DocBDReport.h"
#include "dlgreport.h"
#include "sheetquery.h"
#include "bdattribute.h"
#include "dlgprogress.h"
#include "fileobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocBDReport

IMPLEMENT_DYNCREATE(CDocBDReport, CDocReport)

BEGIN_MESSAGE_MAP(CDocBDReport, CDocReport)
	//{{AFX_MSG_MAP(CDocBDReport)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocBDReport diagnostics

#ifdef _DEBUG
void CDocBDReport::AssertValid() const
{
	CDocReport::AssertValid();
}

void CDocBDReport::Dump(CDumpContext& dc) const
{
	CDocReport::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocBDReport serialization

void CDocBDReport::Serialize(CArchive& ar)
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

CDocBDReport::CDocBDReport()
{
   m_pQueryResult = NULL;
   m_pQuery = NULL;
}

CDocBDReport::~CDocBDReport()
{
   if (m_pQueryResult != NULL) delete m_pQueryResult;
   if (m_pQuery != NULL) delete m_pQuery;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDocBDReport::OnNewDocument()
{	   
   BOOL bQuery = FALSE;

   // Initialise report dialog
 
   if (BDGetPreviousReport() != CNRDB::reportquery)
   {
	   CDlgReport dlg(CDlgReport::Report);
      int nRet = dlg.DoModal();

      // Standard report

	   if (nRet == IDOK)
      {
	       m_pQuery = new CQuery(dlg.GetFType(), dlg.GetAttr(), dlg.GetFeatures(), 
                                BDFEATURE);
		 
          BDSetPreviousReport(CNRDB::standardreport);

      } 
      else if (nRet == IDC_QUERY)
      {
         bQuery = TRUE;
      }
      else
      {
         return FALSE;
      }
   } else
   {
      bQuery = TRUE;
   }

   // Query

   if (bQuery)
   {           
      m_pQuery = new CQuery;
      CSheetQuery dlg(m_pQuery, CSheetQuery::Text, BDString(IDS_REPORT)); 
      int nRet = dlg.DoModal();         
      if (nRet != IDOK)
      {
         return FALSE;
      }
      BDSetPreviousReport(CNRDB::reportquery);

   }  

   CDlgProgress dlgProgress;

 // If a query then delete retrieve data
   
   if (m_pQuery != NULL)
   {               
      BeginWaitCursor();

      m_pQueryResult = new CQueryResult;
      if (!m_pQueryResult->Initialise(m_pQuery))
      {
         if (AfxMessageBox(BDString(IDS_QUERYERROR), MB_YESNO) == IDYES)
         {
            return OnNewDocument();
         }
         return FALSE;
      }      
      EndWaitCursor();

      // Set title 

      m_sTitle = m_pQuery->GetQueryName();
      
   };

	// Create the document

	if (!CDocReport::OnNewDocument())
		return FALSE;

   // Set title

   SetTitle(BDString(IDS_REPORT) + " - " + m_sTitle);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDocBDReport commands

BOOL CDocBDReport::WriteReport()
{
   BOOL bOK = TRUE;
   CFeatureType ftype;
  
   // Write queries

   if (m_pQueryResult != NULL) 
   {
      return WriteReportQuery();      
   };
   return FALSE;
};

/////////////////////////////////////////////////////////////////////////////

BOOL CDocBDReport::WriteReportQuery(BOOL bTitle)
{
   BOOL bOK = TRUE;   
   CString sDate, sTime;

   BDProgressText("Producing report...");

   // Display feature type name   

   if (bTitle)
   {      
	   NewHeader(3);	   
	   Write(m_pQuery->GetQueryName());
	   EndHeader(3);
   };
   
   // If table contains statistics then display original table first

   if (m_pQueryResult->GetQueryResult() != NULL)
   {
      int iAttr = m_pQueryResult->GetQueryResult()->GetAttrArray().GetSize();

      BeginTable(iAttr);

      WriteHeaderQuery(m_pQueryResult->GetQueryResult());
      WriteDataQuery(m_pQueryResult->GetQueryResult());

      EndTable();

      NewLine();
   }
   
   // Determine number of selected features

   int iAttr = m_pQueryResult->GetAttrArray().GetSize();

   BeginTable(iAttr);

   WriteHeaderQuery(m_pQueryResult);
   WriteDataQuery(m_pQueryResult);
   
   EndTable();

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDocBDReport::WriteHeaderQuery(CQueryResult* pQueryResult)
{
   BOOL bOK = TRUE;

   CAttrArray aAttr;
   long lFType = 0;

   // Initialise table

   BeginTableRow();      
      
   // Initialise column headings

   if (pQueryResult->GetSize() > 0)
   {      
      CQueryAttrArray* pAttrArray = pQueryResult->GetAt(0);
      int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
      {
         AddTableCell(pAttrArray->GetAt(i)->GetDesc(), "colheader");
      }
   };

   EndTableRow();         

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDocBDReport::WriteDataQuery(CQueryResult* pQueryResult)
{
   BOOL bOK = TRUE;
   CDateTime datetime;
   CString s;   

   BDProgressRange(0, pQueryResult->GetSize());
   
   int i = 0; for (i = 0; i < pQueryResult->GetSize(); i++)
   {    
      BeginTableRow();
      
      CQueryAttrArray* pAttrArray = pQueryResult->GetAt(i);
      
      // Output attributes for each column
      
      int j = 0; for (j = 0; j < pAttrArray->GetSize(); j++)
      {  
         CAttribute* pAttr = pAttrArray->GetAt(j);

		   CBDAttribute* pBDAttr = (CBDAttribute*)pAttr;
         
         s = pBDAttr->AsString();

         // Hotlink

         if (pAttr->GetDataType() == BDHOTLINK)
         {
            s = FileAsHREF(s);
            AddTableCell(s, "data",FLG_HTML);         
         } 
         
         // Files

         else if (pAttr->GetDataType() == BDFILE)
         {                       
            CFileObj fileobj;
            fileobj.Initialise(*pAttr->GetLongBinary());

            s.Format("<A HREF=\"\?queryresult=%i&attrid=%i\">%s</a>",i,j,fileobj.GetFileName());            
            AddTableCell(s, "data",FLG_HTML);         
         }

         // Other attributes

         else
         {            
            AddTableCell(s, "data");         
         }
      }

      BDProgressPos(i);

      EndTableRow();
   } 
   
   BDProgressRange(0, 0);

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Takes a file name, including path and returns a href
//

CString CDocBDReport::FileAsHREF(CString sPath)
{
   sPath.TrimRight();   

   // If the path contains no backslash then append the directory

   if (sPath.Find('\\') == -1 && sPath.Find('/') == -1)
   {
      sPath = BDGetAppPath() + sPath;
   };

   CString sFile = sPath.Mid(sPath.ReverseFind('\\')+1);
   return "<A HREF=\"" + sPath + "\">" + sFile + "</A>";
}

///////////////////////////////////////////////////////////////////////////////
//
// Tidy up
//

void CDocBDReport::OnCloseDocument() 
{      
   CDocReport::OnCloseDocument();
}

///////////////////////////////////////////////////////////////////////////////

void CDocBDReport::OnFileSaveAs() 
{
   BOOL bOK = TRUE;
   CFileDialog dlg(FALSE, "htm",NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                   "Webpage (*.htm)|*.htm|Text file (*.txt)|*.txt|");

   if (dlg.DoModal() == IDOK)
   {
      BeginWaitCursor();

      // Write the results to a tab separated table

      if (dlg.GetFileExt() == "txt")
      {
         FILE* pFile = fopen(dlg.GetPathName(), "w");
         if (pFile != NULL)
         {
            if (m_pQueryResult->GetQueryResult() != NULL)
            {
               WriteTextfile(pFile, m_pQueryResult->GetQueryResult());
               fprintf(pFile,"\n");
            }
            WriteTextfile(pFile, m_pQueryResult);
                        
            if (pFile != NULL) fclose(pFile);
         } else
         {
            bOK = FALSE;
         }
      } 
      
      // Copy the html file

      else
      {
      
         // Open files

         FILE* pFile = fopen(dlg.GetPathName(), "w");
         FILE* pFileIn = fopen(m_sFileName, "r");

         // Copy one file to the other

         if (pFile != NULL && pFileIn != NULL)
         {
            char ch = fgetc(pFileIn);   
            while (ch != EOF && bOK)
            {
               if (fputc(ch, pFile) == EOF)
               {
                  bOK = FALSE;
               }
               ch = fgetc(pFileIn);   
            }
         } else
         {
            bOK = FALSE;
         }

         // Tidy up

         if (pFile != NULL) fclose(pFile);
         if (pFileIn != NULL) fclose(pFileIn);
      };
      EndWaitCursor();

      if (!bOK)
      {
         AfxMessageBox(BDString(IDS_ERRORWRITE) + ": " + dlg.GetPathName());
      }

   }	
}

///////////////////////////////////////////////////////////////////////////////

void CDocBDReport::WriteTextfile(FILE* pFile, CQueryResult* pQueryResult)
{
   // Write headers

   if (pQueryResult->GetSize() > 0)
   {      
      CQueryAttrArray* pAttrArray = pQueryResult->GetAt(0);
      int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
      {
         fprintf(pFile, "%s\t", pAttrArray->GetAt(i)->GetDesc());
      }               
      fprintf(pFile, "\n");
   };

      // Write data

   int i = 0; for (i = 0; i < pQueryResult->GetSize(); i++)
   {
      CQueryAttrArray* pAttrArray = pQueryResult->GetAt(i);

      int j = 0; for (j = 0; j < pAttrArray->GetSize(); j++)
      {  
         CAttribute* pAttr = pAttrArray->GetAt(j);

	      CBDAttribute* pBDAttr = (CBDAttribute*)pAttr;

         fprintf(pFile, "%s\t", pBDAttr->AsString());
      }
      fprintf(pFile, "\n");            
   };
}


