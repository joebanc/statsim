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

#ifndef _DOCREPORT_H_
#define _DOCREPORT_H_

#include <fstream>

/////////////////////////////////////////////////////////////////////////////
// CDocReport document

#define LEFT         0x0001
#define RIGHT        0x0002
#define CENTRE       0x0004

#define FONT_PLAIN   0x0000
#define FONT_BOLD    0x0001
#define FONT_ITALIC  0x0002
#define FONT_FIXED   0x1000
#define FONT_SMALL   0x0010

#define FLG_NONE     0x0000
#define FLG_HTML     0x0001

#define STYLE_DATA 0x0001

///////////////////////////////////////////////////////////////////////////////

struct CReportItem
{
   CString m_sDesc;
   CString m_sValue;
};

///////////////////////////////////////////////////////////////////////////////
//
// class CDocReport
//
// Base class for overidables relating to reports.  
// Nb. This is not a pure virtual class as IMPLEMENT_DYNCREATE does not
//     work with such classes
//

class CDocReport : public CDocument
{
   DECLARE_SERIAL(CDocReport)
protected:
   CDocReport();        // protected constructor used by dynamic creation

// Attributes
public:
	std::fstream m_strReport;   
     CString m_sFileName;
     char m_sBuffer[128];
     int m_nCols;
     int m_nAlign;
     int m_nFont;     
	  int m_nRow;
     int m_nStyle;

     static CArray <CReportItem, CReportItem> m_aReportItem;
     static void Initialise();
     
     
// Operations
public:                  

// Overidables

     LPCSTR GetFileName() {return m_sFileName;}        

     virtual BOOL CreateReport();
     
     virtual BOOL WriteReport() {ASSERT(FALSE); return FALSE;}

// Initialisation
     BOOL BeginReport();
     BOOL EndReport();

// Meta commands apply only to the current line
          
    enum {fonton=1, fontoff=2};
    enum {styleon=1, styleoff=0};

    void SetAlignment(int nAlign);
    void SetFont(int nFont, int iFlag = fonton|fontoff);
    void SetStyle(int nStyle, int iFlag = fonton|fontoff);

// Member functions
          
    void Write(double, int nDecPlace = 0);
    void Write(CString string, int nFlags = FLG_NONE);
    void Write(long);
     
	 void BeginTable(int nCols);
	 void EndTable();
    void AddTableCell(LPCSTR, CString sClass="", int nFlags=FLG_NONE);
    void BeginTableRow();
    void EndTableRow();    
     
    void Bold();
    void Italic();
	 void Plain();        
    void NewPara(CString sClass="");	 
    void EndPara();
	void NewHeader(int i);
	void EndHeader(int i);
    void NewLine();
	 void Divider();

// Implementation
protected:   

   virtual ~CDocReport();
   virtual void Serialize(CArchive& ar);  // overridden for document i/o
   virtual  BOOL OnNewDocument();
   virtual void OnCloseDocument();
   virtual BOOL SaveModified();

   // Generated message map functions
protected:
   //{{AFX_MSG(CDocReport)   
   afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);   
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif
