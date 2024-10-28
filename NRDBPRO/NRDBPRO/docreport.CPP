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
#include <float.h>

#include "nrdb.h"
#include "docreport.h"
#include "utils.h"
#include "bdhtmlview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Static Variable
//

CArray <CReportItem, CReportItem> CDocReport::m_aReportItem;

/////////////////////////////////////////////////////////////////////////////
// CDocReport

IMPLEMENT_SERIAL(CDocReport, CDocument, 0 /* schema number*/ )

CDocReport::CDocReport()
{
   m_nCols = 0;
   m_nFont = 0;
   m_nStyle = 0;
   m_nAlign = 0;
   m_nRow = 0;

}

///////////////////////////////////////////////////////////////////////////////

void CDocReport::Initialise()
{
      // Initialise CDocReport

   CReportItem item;
   m_aReportItem.RemoveAll();

   item.m_sDesc = "[" + BDString(IDS_ORGANISATION) + "]";
   item.m_sValue = BDGetSettings().m_Organization;
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_ADDRESS) + "]";
   item.m_sValue = BDGetSettings().m_Address;
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_CITY) + "]";
   item.m_sValue = BDGetSettings().m_City;
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_PROVINCE) + "]";
   item.m_sValue = BDGetSettings().m_Province;
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_COUNTRY) + "]";
   item.m_sValue = BDGetSettings().m_Country;
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_TELEPHONE) + "]";
   item.m_sValue = BDGetSettings().m_Telephone;
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_FAX) + "]";
   item.m_sValue = BDGetSettings().m_Fax;
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_EMAIL) + "]";
   item.m_sValue = "<a href=\"mailto:"+ BDGetSettings().m_Email + "\">" + BDGetSettings().m_Email + "</a>";
   m_aReportItem.Add(item);

   item.m_sDesc = "[" + BDString(IDS_LOGO) + "]";
   CString sLogo = BDGetSettings().m_Logo;
   if (sLogo.Find('\\') == -1) sLogo = BDGetAppPath() + sLogo;
   item.m_sValue = "<img src=\"" + sLogo + "\">";
   m_aReportItem.Add(item);  

}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDocReport::OnNewDocument()
//

BOOL CDocReport::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   TRY 
   {
      if (!CreateReport()) return FALSE;

   }
   CATCH(CException, ex)
   {
      AfxMessageBox(BDString(IDS_ERRORREPORT));
   }
   END_CATCH

   // Force OnClose message

    SetModifiedFlag(TRUE);

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
CDocReport::~CDocReport()
{
   // delete file on exit

   remove(m_sFileName);
}


BEGIN_MESSAGE_MAP(CDocReport, CDocument)
   //{{AFX_MSG_MAP(CDocReport)   
   ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)   
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDocReport serialization

void CDocReport::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
   {      
   }
   else
   {    
   }
}

/////////////////////////////////////////////////////////////////////////////
// CDocReport commands

///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::NewPara()
//

void CDocReport::NewPara(CString sClass)
{  
   if (m_nStyle == STYLE_DATA) m_strReport << "<P class=\"data\">";   
   else if (sClass == "") m_strReport << "<P>";
   else m_strReport << "<P class=\"" << sClass << "\">";
   
}                       

void CDocReport::EndPara()
{  
   m_strReport << "</P>\n";
}                       

///////////////////////////////////////////////////////////////////////////////

void CDocReport::NewHeader(int i)
{
   m_strReport << "<H" << i << ">";
}

void CDocReport::EndHeader(int i)
{
   m_strReport << "</H" << i << ">\n";
};

void CDocReport::NewLine()
{  
   m_strReport << "<BR>\n";
}                       

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDocReport::BeginReport();

BOOL CDocReport::BeginReport()
{        
  // Create a temporary file name
      
   m_sFileName = ::GetTempFileName();

  // Open temporary file
  
   m_strReport.open(m_sFileName, std::ios::out);

   m_strReport << "<HTML>\n";  

   m_strReport << "<head>\n";
   
   // Output code page so that extended characters are displayed correctly

   m_strReport << "<meta http-equiv=Content-Type content=\"text/html; charset=windows-1252\">\n";

   // Set the style sheet

   CString s = BDGetSettings().m_sStyleSheet;
   if (s == "") s = "nrdbpro.css";
   if (s.Find('\\') == -1) s = BDGetAppPath() + s; 

   m_strReport << "<link rel=\"stylesheet\" type=\"text/css\" href=\"";   
   m_strReport << s + "\" />\n";
   m_strReport << "</head>\n";	

   m_strReport << "<body>\r\n";


   // Custom Header   

   if (BDGetSettings().m_bCustomHeader)
   {
      CString s = BDGetSettings().m_sCustomHeader;

      // Replace the strings

      CDocReport::Initialise();
      
      int i = 0; for (i = 0; i < m_aReportItem.GetSize(); i++)
      {
         int pos;
         while ((pos = s.Find(m_aReportItem[i].m_sDesc)) != -1)
         {
            s = s.Left(pos) + m_aReportItem[i].m_sValue + s.Mid(pos + m_aReportItem[i].m_sDesc.GetLength());
         }
      }
      m_strReport << s;
   } 

   // Default Header
   else
   {
      CString sLogo = BDGetSettings().m_Logo;
      if (sLogo.Find('\\') == -1) sLogo = BDGetAppPath() + sLogo;

      m_strReport << "<table width=80%><tr><td width=20%>\n";
      m_strReport << "<p><img src=\"" << sLogo << "\"\n";
      m_strReport << " height=75, align=center></p>\n";
      m_strReport << "<td width=60%>\n";

      m_strReport << "<center><p class=header>\n";
  
      if (!BDGetSettings().m_Organization.IsEmpty())
      {
         m_strReport << "<b>";
         Write(BDGetSettings().m_Organization);
         m_strReport << "</b><br>\n";
      };

      if (!BDGetSettings().m_Address.IsEmpty())
      {
         Write(BDGetSettings().m_Address);
         m_strReport << "<br>\n";
      };

      if (!BDGetSettings().m_City.IsEmpty())
      {
         Write(BDGetSettings().m_City);
         m_strReport << "<br>\n";
      };

      if (!BDGetSettings().m_Province.IsEmpty())
      {
         Write(BDGetSettings().m_Province);   
         m_strReport << "<br>\n";
      };

      if (!BDGetSettings().m_Country.IsEmpty())
      {
         Write(BDGetSettings().m_Country);
         m_strReport << "<br>\n";      
      };
      m_strReport << "</p></center>\n";	   

      m_strReport << "</table>\n";
   };
   
   
   return !m_strReport.fail();
};

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDocReport::EndReport()
//   

BOOL CDocReport::EndReport()
{  
   // Note: font size must be moved outside of <p class="footer" so that
   // style sheet is not overidden

   // Custom footer

   if (BDGetSettings().m_bCustomFooter)
   {
      CString s = BDGetSettings().m_sCustomFooter;

      // Replace the strings

      CDocReport::Initialise();
      
      int i = 0; for (i = 0; i < m_aReportItem.GetSize(); i++)
      {
         int pos;
         while ((pos = s.Find(m_aReportItem[i].m_sDesc)) != -1)
         {
            s = s.Left(pos) + m_aReportItem[i].m_sValue + s.Mid(pos + m_aReportItem[i].m_sDesc.GetLength());
         }
      }
      m_strReport << s;
   } 

   // Default footer
   else
   {
      m_strReport << "<CENTER><p class=\"footer\">";   
      if (!BDGetSettings().m_Telephone.IsEmpty()) Write(BDString(IDS_TELEPHONE) + ": " + BDGetSettings().m_Telephone);
      if (!BDGetSettings().m_Fax.IsEmpty()) Write( " " + BDString(IDS_FAX) + ": " + BDGetSettings().m_Fax);
      m_strReport << "<BR>\n";
      if (!BDGetSettings().m_Email.IsEmpty()) m_strReport << BDString(IDS_EMAIL) + ": <A HREF='mailto:" << BDGetSettings().m_Email << "'>" << BDGetSettings().m_Email << "</A><BR>\n";
      m_strReport << "</p></CENTER>\n";
   };

   m_strReport << "</body>\n";
   m_strReport << "</HTML>\n";

   m_strReport.close();   
   
   return !m_strReport.fail();
};               

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDocReport::CreateReport()
//

BOOL CDocReport::CreateReport()
{
   BeginWaitCursor();
   BeginReport();
   BOOL bOK = WriteReport();
   EndReport();
   EndWaitCursor();
   return bOK;
};

///////////////////////////////////////////////////////////////////////////////
//               
// CDocReport::Write()
// 

void CDocReport::Write(CString string, int nFlags)
{
// String out reserved characters from string

   if (nFlags != FLG_HTML)
   {   
	   // Nb: Must change ampersand first as contained in reserved characters
      int i=0;
      while ((i = string.Find('&',i)) != -1)
      {
	      string = string.Left(i) + "&amp;" + string.Mid(i+1);
	      i++;
      }      
   
      while ((i = string.Find('<')) != -1)
      {
	      string = string.Left(i) + "&lt;" + string.Mid(i+1);
      }
      while ((i = string.Find('>')) != -1)
      {
	      string = string.Left(i) + "&gt;" + string.Mid(i+1);
      }
      while ((i = string.Find('\"')) != -1)
      {
	      string = string.Left(i) + "&quot;" + string.Mid(i+1);
      }
   };

   m_strReport << string;          
}; 


///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::Write(double, int nDecPlace = 0)
//

void CDocReport::Write(double dValue, int nDecPlace)
{  
   BOOL bOK = TRUE;
    
   m_strReport.flags(std::ios::left);   
      
   if (_snprintf(m_sBuffer, sizeof(m_sBuffer), "%-.*lf", nDecPlace, dValue) <= 0)
   {
      bOK = FALSE;
   }
   m_strReport << m_sBuffer;   
  
  // Ensure output to stream okay
   
   if (m_strReport.fail() || !bOK)
   {  
      AfxThrowUserException();
      bOK = FALSE;
   }         
};

///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::Write(long)
//

void CDocReport::Write(long lValue)
{   
   m_strReport.flags(std::ios::left);
         
   m_strReport << lValue;  
  
  // Ensure output to stream okay
   
   if (m_strReport.fail())
   {  
      AfxThrowUserException();   
   }      
};
   
///////////////////////////////////////////////////////////////////////////////

void CDocReport::BeginTable(int nCols)
{	
   m_nCols = nCols;
   m_nRow = 0;
   m_strReport << "<TABLE class=\"body\" border=\"1\" cellpadding=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" WIDTH=\"100%\">\n";
}

///////////////////////////////////////////////////////////////////////////////

void CDocReport::EndTable()
{
	m_nCols = 0;

   m_strReport << "</TABLE>\n";
}

///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::SetFont(int nFont)
//
// Changes the font used to display
//

void CDocReport::SetFont(int nFont, int iFlag)
{
   // Clear previous settings
   
   if (iFlag & fontoff)
   {
      if (m_nFont & FONT_BOLD) m_strReport << "</B>";  
      if (m_nFont & FONT_ITALIC) m_strReport << "</I>";  
      if (m_nFont & FONT_FIXED) m_strReport << "</FONT>";  
      if (m_nFont & FONT_SMALL) m_strReport << "</FONT>";  
   };

   // Set new settings

   m_nFont = nFont;

   if (iFlag & fonton)
   {
      if (m_nFont & FONT_BOLD) m_strReport << "<B>";  
      if (m_nFont & FONT_ITALIC) m_strReport << "<I>";  
      if (nFont & FONT_FIXED) m_strReport << "<FONT FACE=\"Courier New\">";
      if (nFont & FONT_SMALL) m_strReport << "<FONT SIZE=2>";
   };    
};

///////////////////////////////////////////////////////////////////////////////

void CDocReport::SetStyle(int nStyle, int iFlag)
{
     // Clear previous settings   

   if (iFlag & fontoff)
   {
      m_nStyle = 0;
   } else
   {
      m_nStyle = nStyle;
   }
};

///////////////////////////////////////////////////////////////////////////////
//
// Adds string to a list of values to be displayed as column headings
//

void CDocReport::AddTableCell(LPCSTR lpString, CString sClass, int nFlags)
{  
   int nWidth = 100 / max(1,m_nCols);   
   m_strReport << "<TD WIDTH=" << nWidth << "%>";
   SetFont(m_nFont, fonton);
   NewPara(sClass);
   Write(lpString, nFlags);   
   EndPara();
   SetFont(m_nFont, fontoff);
   m_strReport << "&nbsp</TD>\n";   
}

///////////////////////////////////////////////////////////////////////////////

void CDocReport::BeginTableRow()
{
   if (m_nRow == 0) m_strReport << "<TR class=\"header\">";   
   else if (m_nRow%2) m_strReport << "<TR class=\"odd\">";   
   else m_strReport << "<TR class=\"even\">";     

   m_nRow++;
}

///////////////////////////////////////////////////////////////////////////////
//
// Outputs the column headings, splitting them onto multiple lines as necessary
//

void CDocReport::EndTableRow()
{
   m_strReport << "</TR>\n";  
};

///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::Bold()
//

void CDocReport::Bold()
{
   SetFont(FONT_BOLD);
}

///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::Italic()
//

void CDocReport::Italic()
{
   SetFont(FONT_ITALIC);
};

///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::Plain()
//

void CDocReport::Plain()
{
   SetFont(FONT_PLAIN);   
};

///////////////////////////////////////////////////////////////////////////////
//
// void CDocReport::SetAlignment(int nAlign)
//
// Sets the allignment (justification) for columns
//

void CDocReport::SetAlignment(int nAlign)
{
   // Reset previous alignment

   if (m_nAlign & RIGHT) m_strReport << "</RIGHT>";
   if (m_nAlign & CENTRE) m_strReport << "</CENTER>";
   if (m_nAlign & LEFT) m_strReport << "</LEFT>";

   m_nAlign = nAlign;

   if (m_nAlign & RIGHT) m_strReport << "<RIGHT>";
   if (m_nAlign & CENTRE) m_strReport << "<CENTER>";
   if (m_nAlign & LEFT) m_strReport << "<LEFT>";
};

///////////////////////////////////////////////////////////////////////////////

void CDocReport::Divider()
{
}



///////////////////////////////////////////////////////////////////////////////

void CDocReport::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
//
// Before closing give the html view time to close any embedded windows
//

BOOL CDocReport::SaveModified()
{
   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      CBDHtmlView* pView = (CBDHtmlView*)GetNextView(pos);
      if (pView->IsKindOf(RUNTIME_CLASS(CBDHtmlView))) 
      {
         if (!pView->PreCloseView()) 
         {            
            return FALSE;
         }
      };
   }

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void CDocReport::OnCloseDocument()
{   
   CDocument::OnCloseDocument();
}
